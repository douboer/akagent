package monitor

import "akagent/setting"

func MonitorStart(){
	if setting.FileMonitorIsUp {
		fileMonitor:= NewFileMonitor()
		fileMonitor.MonitorStart()
	}
	if setting.PsMonitorIsUp {
		processMonitor:= NewProcessMonitor()
		processMonitor.MonitorStart()
	}
}

