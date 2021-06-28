package monitor

import "akagent/setting"

func MonitorStart(cfg setting.Config){
	if cfg.FileCfg.Enable {
		fileMonitor:= NewFileMonitor(cfg)
		fileMonitor.MonitorStart()
	}
	if cfg.ProcessCfg.Enable {
		processMonitor:= NewProcessMonitor(cfg)
		processMonitor.MonitorStart()
	}
	if cfg.NetCfg.Enable {
		netMonitor:= NewNetMonitor(cfg)
		netMonitor.MonitorStart()
	}
}

