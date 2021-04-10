package monitor

import (
	"akagent/akfs"
	"akagent/setting"
	"akagent/src/report"
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"os/user"
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
	FileEvent FileEvent
	ReportType	string
	ReportHost	string
	ReportPort	int
}

func NewFileMonitor() *FileMonitor {
	return &FileMonitor{
		ReportType:setting.ReportType,
		ReportHost:setting.ReportHost,
		ReportPort:setting.ReportPort,
		HttpReport:report.NewHttpReport(),
	}
}

func (f *FileMonitor)MonitorStart(){

	akfs.FileMonitor()

	data := make([]byte, 2048)
	localaddress, _ := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d","127.0.0.1",setting.FileUsedPort))
	udplistener, err := net.ListenUDP("udp", localaddress)
	if err != nil {
		log.Print(err.Error())
		return
	}
	defer udplistener.Close()

	for {
		n, _, err := udplistener.ReadFromUDP(data[0:])
		if err != nil {
			log.Println(err.Error())
			return
		}
		f.Analy(data[0:n])
		f.Report()
	}
}

func (p *FileMonitor)Analy(data []byte){
	p.FileEvent.New(data)
}

//Filter 添加事件监控过滤规则
func (p *FileMonitor)Filter() bool {
	return true
}

func (p *FileMonitor) Report() {
	bytesData, _ := json.Marshal(p.FileEvent)

	log.Print(string(bytesData))
	if p.ReportType == "https" {
		p.HttpReport.Content = bytesData
		p.HttpReport.TargetUrl = fmt.Sprintf("https://%s:%d/log/hids/monitor/file",p.ReportHost,p.ReportPort)
		p.HttpReport.Post()
	}
}