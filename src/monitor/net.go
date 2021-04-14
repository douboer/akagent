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

type NetMonitor struct {
	HttpReport *report.HttpReport
	NetEvent NetEvent
	ReportType	string
	ReportHost	string
	ReportPort	uint16
}

func NewNetMonitor() *NetMonitor {
	return &NetMonitor{
		ReportType:setting.ReportType,
		ReportHost:setting.ReportHost,
		ReportPort:uint16(setting.ReportPort),
		HttpReport:report.NewHttpReport(),
	}
}

func (nm *NetMonitor)MonitorStart(){

	go func() {
		akfs.NetMonitor()

		data := make([]byte, 2048)
		localaddress, _ := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d","127.0.0.1",setting.NetUsedPort))
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
			nm.Analy(data[0:n])
			if nm.Filter() {
				nm.Report()
			}

		}
	}()
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
	return true
}

func (n *NetMonitor) Report() {
	bytesData, _ := json.Marshal(n.NetEvent)

	log.Print(string(bytesData))
	if n.ReportType == "https" {
		n.HttpReport.Content = bytesData
		n.HttpReport.TargetUrl = fmt.Sprintf("https://%s:%d/log/hids/monitor/net",n.ReportHost,n.ReportPort)
		n.HttpReport.Post()
	}

}

//NetEvent 网络监控接口字段
type NetEvent struct {
	Timestamp uint64 `json:"timestamp"`                       //事件时间戳
	Data_type uint32 `json:"data_type" enum:"1001,1002,1003"` //数据类型
	Pid       uint32 `json:"pid"`                             //进程id
	Uid       uint32 `json:"uid"`                             //用户ID
	UserName  string `json:"user_name"`                       //用户名
	Gid       uint32 `json:"gid"`                             //用户组ID
	GroupName string `json:"group_name"`                      //组名
	Namespace uint32 `json:"namespace"`                       //进程命名空间ID
	SrcIp	string	`json:"src_ip"`							//源地址
	DstIp	string	`json:"dst_ip"`							//目的地址
	SrcPort	uint16	`json:"src_port"`						//源端口
	DstPort	uint16	`json:"dst_port"`						//目的端口
	Exe_file  string `json:"exe_file"`                        //进程文件
}

//NetEvent 网络数据解析
func (n *NetEvent) New(monitorData []byte) {
	offset := 0
	n.Timestamp = binary.LittleEndian.Uint64(monitorData[offset : offset+8])
	offset += 8

	n.Data_type = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	n.Pid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

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


	n.Namespace = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4

	//网络四元组
	SrcIp := binary.BigEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	n.SrcIp = InetNtoA(SrcIp)

	DstIp := binary.BigEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	n.DstIp = InetNtoA(DstIp)

	n.SrcPort = binary.BigEndian.Uint16(monitorData[offset : offset+2])
	offset += 2
	n.DstPort = binary.BigEndian.Uint16(monitorData[offset : offset+2])
	offset += 2



	n.Exe_file = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))
	offset += 256


}

func InetNtoA(ip uint32) string {
	return fmt.Sprintf("%d.%d.%d.%d",
		byte(ip>>24), byte(ip>>16), byte(ip>>8), byte(ip))
}