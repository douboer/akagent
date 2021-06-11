package monitor

import (
	"akagent/akfs"
	"akagent/setting"
	"akagent/src/filter"
	"akagent/src/report"
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"log"
	"net"
	"os/user"
	"runtime"
)

type NetMonitor struct {
	HttpReport *report.HttpReport
	NetEvent NetEvent
	ReportType	string
	ReportHost	string
	ReportPort	uint16
	Name	string
}

func NewNetMonitor() *NetMonitor {
	return &NetMonitor{
		Name: "net",
		ReportType:setting.ReportType,
		ReportHost:setting.ReportHost,
		ReportPort:uint16(setting.ReportPort),
		HttpReport:report.NewHttpReport(),
	}
}

func (p *NetMonitor)MonitorStart(){

	go func() {
		akfs.NetMonitor()

		localaddress, _ := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d","127.0.0.1",setting.NetUsedPort))
		udplistener, err := net.ListenUDP("udp", localaddress)
		if err != nil {
			log.Print(err.Error())
			return
		}
		defer udplistener.Close()

		for {
			p.readfs(udplistener)
		}
	}()
}

func (p *NetMonitor)readfs(udpConn *net.UDPConn){
	data := make([]byte, 2048)
	n, _, err := udpConn.ReadFromUDP(data)
	if err != nil {
		log.Println(err.Error())
		return
	}

	p.Analy(data[0:n])
	if p.Filter() {
		p.Report()
	}
}

func (n *NetMonitor)Analy(data []byte){
	n.NetEvent.New(data)
}

//Filter 添加事件监控过滤规则
func (n *NetMonitor)Filter() bool {
	switch  {
	case n.NetEvent.DstIp == n.ReportHost && n.NetEvent.DstPort == n.ReportPort:  //过滤事件上报日志
		return false
	}

	for _,v := range filter.FilterMap[runtime.GOOS][n.Name]{
		if v.Match(&n.NetEvent) {
			return false
		}
	}

	return true
}

func (n *NetMonitor) Report() {

	bytesData, _ := json.Marshal(n.NetEvent)
	log.Print(string(bytesData))

	if setting.ReportEnable != true{
		return
	}
	if n.ReportType == "https" {
		n.HttpReport.Content = bytesData
		n.HttpReport.TargetUrl = fmt.Sprintf("https://%s:%d/log/hids/monitor/net",n.ReportHost,n.ReportPort)
		n.HttpReport.Post()
	}

}

//NetEvent 网络监控接口字段
type NetEvent struct {
	Timestamp   uint64 `json:"timestamp"`                       //事件时间戳
	Data_type   uint32 `json:"data_type" enum:"3001,3002,3003"` //数据类型
	Pid         uint32 `json:"pid"`                             //进程id
	Exe_file    string `json:"exe_file"`                        //进程文件
	Parent_pid  uint32 `json:"parent_pid"`                      //父进程id
	Parent_name string `json:"parent_name"`                     //父进程文件
	Ptgid       uint32 `json:"ptgid"`                           //进程tgid
	Uid         uint32 `json:"uid"`                             //用户ID
	UserName    string `json:"user_name"`                       //用户名
	Gid         uint32 `json:"gid"`                             //用户组ID
	GroupName   string `json:"group_name"`                      //组名
	Namespace   uint32 `json:"namespace"`                       //进程命名空间ID
	SrcIp       string `json:"src_ip"`                          //源地址
	DstIp       string `json:"dst_ip"`                          //目的地址
	SrcPort     uint16 `json:"src_port"`                        //源端口
	DstPort     uint16 `json:"dst_port"`                        //目的端口
}


//NetEvent 网络数据解析
func (n *NetEvent) New(monitorData []byte) {
	offset := 0
	n.Timestamp = binary.LittleEndian.Uint64(monitorData[offset : offset+8])
	offset += 8
	//1
	n.Data_type = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	n.Pid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	n.Parent_pid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	n.Ptgid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	//2
	n.Uid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	usr, err := user.LookupId(fmt.Sprintf("%d", n.Uid))
	if err == nil {
		n.UserName = usr.Username
	}

	n.Gid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	Ginfo, err := user.LookupGroupId(fmt.Sprintf("%d", n.Gid))
	if err == nil {
		n.GroupName = Ginfo.Name
	}

	//3
	n.Namespace = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	//网络四元组
	SrcIp := binary.BigEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	n.SrcIp = IP2String(SrcIp)
	//4
	DstIp := binary.BigEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	n.DstIp = IP2String(DstIp)

	n.SrcPort = binary.BigEndian.Uint16(monitorData[offset : offset+2])
	offset += 2
	n.DstPort = binary.BigEndian.Uint16(monitorData[offset : offset+2])
	offset += 2

	//5
	n.Parent_name = string(bytes.Trim(monitorData[offset:offset+64], "\x00"))
	offset += 64

	n.Exe_file = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))
	offset += 256

}

func IP2String(ip uint32) string {
	return fmt.Sprintf("%d.%d.%d.%d",
		byte(ip>>24), byte(ip>>16), byte(ip>>8), byte(ip))
}