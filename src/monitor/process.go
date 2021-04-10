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

type ProcessMonitor struct {
	HttpReport *report.HttpReport
	ProcessEvent ProcessEvent
	ReportType	string
	ReportHost	string
	ReportPort	int
}

func NewProcessMonitor() *ProcessMonitor {
	return &ProcessMonitor{
		ReportType:setting.ReportType,
		ReportHost:setting.ReportHost,
		ReportPort:setting.ReportPort,
		HttpReport:report.NewHttpReport(),
	}
}

func (p *ProcessMonitor)MonitorStart(){

	akfs.PsMonitor()

	data := make([]byte, 2048)
	localaddress, _ := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d","127.0.0.1",setting.PsUsedPort))
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
		p.Analy(data[0:n])
		p.Report()
	}
}

func (p *ProcessMonitor)Analy(data []byte){
	p.ProcessEvent.New(data)
}

//Filter 添加事件监控过滤规则
func (p *ProcessMonitor)Filter() bool {
	if p.ProcessEvent.Ppid == 0 {
		return false
	}
	switch  {
	case p.ProcessEvent.Exe_hash == "4d037094cb4d29c0d331caf827df3539":
		return false
	}

	return true
}

func (p *ProcessMonitor) Report() {
	bytesData, _ := json.Marshal(p.ProcessEvent)

	log.Print(string(bytesData))
	if p.ReportType == "https" {
		p.HttpReport.Content = bytesData
		p.HttpReport.TargetUrl = fmt.Sprintf("https://%s:%d/log/hids/monitor/process",p.ReportHost,p.ReportPort)
		p.HttpReport.Post()
	}

}

//ProcessEvent 进程监控接口字段
type ProcessEvent struct {
	Exe_file  string `json:"exe_file"`                        //进程文件
	Exe_hash  string `json:"exe_hash"`                        //文件hash
	Pid       uint32 `json:"pid"`                             //进程id
	Ppid      uint32 `json:"ppid"`                            //父进程ID
	Ptgid	  uint32 `json:"ptgid"`                            //父进程TGID
	Data_type uint32 `json:"data_type" enum:"1001,1002,1003"` //数据类型
	Argv      string `json:"argv"`                            //进程参数
	ParentName string	`json:"parent_name"`				  //父进程名
	Uid       uint32 `json:"uid"`                             //用户ID
	UserName  string `json:"user_name"`                       //用户名
	Gid       uint32 `json:"gid"`                             //用户组ID
	GroupName string `json:"group_name"`                      //组名
	Namespace uint32 `json:"namespace"`                       //进程命名空间ID
	Timestamp uint64 `json:"timestamp"`                       //事件时间戳
}

//ProcessEvent 进程数据解析
func (p *ProcessEvent) New(monitorData []byte) {
	offset := 0
	p.Timestamp = binary.LittleEndian.Uint64(monitorData[offset : offset+8])
	offset += 8

	p.Data_type = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	p.Pid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	p.Ppid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	p.Ptgid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4


	p.Uid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	usr, err := user.LookupId(fmt.Sprintf("%d", p.Uid))
	if err == nil {
		p.UserName = usr.Username
	}


	p.Gid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	Ginfo, err := user.LookupGroupId(fmt.Sprintf("%d", p.Gid))
	if err == nil {
		p.GroupName = Ginfo.Name
	}


	p.Namespace = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	p.ParentName = string(bytes.Trim(monitorData[offset:offset+64], "\x00"))
	offset += 64

	p.Exe_hash = string(bytes.Trim(monitorData[offset:offset+64], "\x00"))
	offset += 64

	p.Exe_file = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))
	offset += 256

	p.Argv = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))

}