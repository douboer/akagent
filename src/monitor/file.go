package monitor

import (
	"akagent/akfs"
	"akagent/setting"
	"akagent/src/filter"
	"akagent/src/report"
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"os/user"
	"path"
	"path/filepath"
	"runtime"
)

//FileEvent 文件监控接口字段
type FileEvent struct {
	Timestamp uint64 `json:"timestamp"`                       //事件时间戳
	Data_type uint32 `json:"data_type" enum:"1001,1002,1003"` //数据类型
	Pid       uint32 `json:"pid"`                             //进程id
	Ppid      uint32 `json:"ppid"`                            //父进程ID
	Ptgid	  uint32 `json:"ptgid"`                            //父进程TGID
	Uid       uint32 `json:"uid"`                             //用户ID
	UserName  string `json:"user_name"`                       //用户名
	Gid       uint32 `json:"gid"`                             //用户组ID
	GroupName string `json:"group_name"`                      //组名
	Namespace uint32 `json:"namespace"`                       //进程命名空间ID
	Exe_file  string `json:"exe_file"`                        //进程文件
	Chg_file  string `json:"chg_file"`                        //变更文件
}

//New 文件数据解析
func (f *FileEvent) New(monitorData []byte) {
	offset := 0
	f.Timestamp = binary.LittleEndian.Uint64(monitorData[offset : offset+8])
	offset += 8

	f.Data_type = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	f.Pid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	f.Ppid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	f.Ptgid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4


	f.Uid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	usr, err := user.LookupId(fmt.Sprintf("%d", f.Uid))
	if err == nil {
		f.UserName = usr.Username
	}


	f.Gid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	Ginfo, err := user.LookupGroupId(fmt.Sprintf("%d", f.Gid))
	if err == nil {
		f.GroupName = Ginfo.Name
	}


	f.Namespace = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	f.Exe_file = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))
	offset += 256

	f.Chg_file = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))
}

type FileMonitor struct {
	HttpReport *report.HttpReport
	Event FileEvent
	Name	string

	config 	setting.Config
}



func NewFileMonitor(cfg setting.Config) *FileMonitor {
	return &FileMonitor{
		Name: "file",
		config: cfg,
		HttpReport:report.NewHttpReport(cfg),
	}
}

func (fM *FileMonitor)MonitorStart(){

	go func() {
		akfs.FileMonitor()

		localaddress, _ := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d","127.0.0.1",fM.config.FileCfg.UseInnerPort))
		udplistener, err := net.ListenUDP("udp", localaddress)
		if err != nil {
			log.Print(err.Error())
			return
		}
		defer udplistener.Close()

		for {
			fM.readfs(udplistener)
		}
	}()
}

func (fM *FileMonitor)readfs(udpConn *net.UDPConn){
	data := make([]byte, 2048)
	n, _, err := udpConn.ReadFromUDP(data)
	if err != nil {
		log.Println(err.Error())
		return
	}

	fM.Analy(data[0:n])
	if fM.Filter() {

		fM.reSave()

		fM.HttpReport.Report(fM.Event,fM.Name)
	}
}

func (fM *FileMonitor)reSave(){
	if fM.config.FileCfg.FileReSave.Enable {
		copyFile(fM.Event.Chg_file,fmt.Sprintf("%s/%s",fM.config.FileCfg.FileReSave.SavePath,path.Base(fM.Event.Chg_file)))
	}
}

func (fM *FileMonitor)Analy(data []byte){
	fM.Event.New(data)
}

//Filter 添加事件监控过滤规则
func (fM *FileMonitor)Filter() bool {
	if setting.SelfName == filepath.Base(fM.Event.Exe_file){
		return false
	}


	if setting.SelfName == filepath.Base(fM.Event.Exe_file) {
		return false
	}

	IsMonitorPath := false

	for k,_ :=range fM.config.FileCfg.MonitorPathMap {
		reled, err := filepath.Rel(k, fM.Event.Chg_file)
		if err != nil {
			continue
		}
		if !(len(reled)>2 && reled[0:2] == "..") {
			IsMonitorPath = true
		}
	}

	if IsMonitorPath != true { //不在指定目录 不监控
		return false
	}


	for _,v := range filter.FilterMap[runtime.GOOS][fM.Name]{
		if v.Match(&fM.Event) {
			return false
		}
	}

	return true
}

func copyFile(dstName, srcName string) (written int64, err error) {
	src, err := os.Open(srcName)
	if err != nil {
		return
	}
	defer src.Close()
	dst, err := os.OpenFile(dstName, os.O_WRONLY|os.O_CREATE, 0644)
	if err != nil {
		return
	}
	defer dst.Close()
	return io.Copy(dst, src)
}