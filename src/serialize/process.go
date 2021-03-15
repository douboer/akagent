package serialize

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"log"
	"os/user"
)

//ProcessMonitor 进程监控接口字段
type ProcessMonitor struct {
	Exe_file  string `json:"exe_file"`                        //进程文件
	Exe_hash  string `json:"exe_hash"`                        //文件hash
	Pid       uint32 `json:"pid"`                             //进程id
	Ppid      uint32 `json:"ppid"`                            //父进程ID
	Data_type uint32 `json:"data_type" enum:"1001,1002,1003"` //数据类型
	Argv      string `json:"argv"`                            //进程参数
	Uid       uint32 `json:"uid"`                             //用户ID
	UserName  string `json:"user_name"`                       //用户名
	Gid       uint32 `json:"gid"`                             //用户组ID
	GroupName string `json:"group_name"`                      //组名
	Namespace uint32 `json:"namespace"`                       //进程命名空间ID
	Timestamp uint64 `json:"timestamp"`                       //事件时间戳

}

//ProcessAnaly 进程数据解析
func (p *ProcessMonitor) NewProcess(monitorData []byte) {
	offset := 0
	p.Data_type = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	p.Pid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	p.Ppid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	p.Uid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])

	usr, err := user.LookupId(fmt.Sprintf("%d", p.Uid))
	if err == nil {
		log.Println(usr)
		p.UserName = usr.Username
	}

	offset += 4
	p.Gid = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	Ginfo, err := user.LookupGroupId(fmt.Sprintf("%d", p.Gid))
	if err == nil {
		log.Println(Ginfo)
		p.GroupName = Ginfo.Name
	}


	offset += 4
	p.Namespace = binary.LittleEndian.Uint32(monitorData[offset : offset+4])
	offset += 4
	p.Timestamp = binary.LittleEndian.Uint64(monitorData[offset : offset+8])
	offset += 8
	p.Exe_hash = string(bytes.Trim(monitorData[offset:offset+64], "\x00"))
	offset += 64
	p.Exe_file = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))
	offset += 256
	p.Argv = string(bytes.Trim(monitorData[offset:offset+256], "\x00"))

}

func (p *ProcessMonitor) Report() {
	bytesData, _ := json.Marshal(p)

	log.Print(string(bytesData))
	//httpreport := report.HttpReport{
	//	Content: bytesData,
	//	TargetUrl: "https://127.0.0.1/monitor/process",
	//}
	//httpreport.Post()
}
