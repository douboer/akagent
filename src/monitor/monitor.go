package monitor

import (
	"akagent/akfs"
	"akagent/src/serialize"
	"log"
	"net"
)

func MonitorStart(){

	akfs.PsMonitor()

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
		Analy(data[0:n])
	}
}

func Analy(data []byte){
	var processMonitor serialize.ProcessMonitor
	processMonitor.NewProcess(data)
	processMonitor.Report()
}