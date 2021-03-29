package monitor

import (
	"akagent/akfs"
	"akagent/src/report"
	"akagent/src/serialize"
	"encoding/json"
	"log"
	"net"
)

func MonitorStart(){

	akfs.PsMonitor()

	var processHandle = &ProcessHandle{
		HttpReport:report.NewHttpReport(),
	}

	data := make([]byte, 2048)
	localaddress, _ := net.ResolveUDPAddr("udp", "127.0.0.1:65432")
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
		processHandle.Analy(data[0:n])
	}
}

type ProcessHandle struct {
	HttpReport *report.HttpReport
}



func (p *ProcessHandle)Analy(data []byte){
	var processMonitor serialize.ProcessMonitor
	processMonitor.NewProcess(data)
	p.Report(processMonitor)
}

func (p *ProcessHandle) Report(processData serialize.ProcessMonitor) {
	bytesData, _ := json.Marshal(processData)
	if processData.Ppid == 0 {
		return
	}
	switch  {
	case processData.Exe_hash == "4d037094cb4d29c0d331caf827df3539":
		return
	}

	log.Print(string(bytesData))

	p.HttpReport.Content = bytesData
	p.HttpReport.TargetUrl = "https://10.10.27.114:25000/log/hids/monitor/process"
	p.HttpReport.Post()
}