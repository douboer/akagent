package setting

import (
	"fmt"
	"gopkg.in/ini.v1"
)

var (
	PsMonitorIsUp	bool
	FileMonitorIsUp	bool
	NetMonitorIsUp	bool

	ReportType	string
	ReportHost	string
	ReportPort	int

	PsUsedPort	int
	FileUsedPort	int
	NetUsedPort	int
)

func init() {
	file, err := ini.Load("config.ini")
	if err != nil {
		fmt.Println("配置文件读取错误，请检查文件路径:", err)
	}
	LoadMonitor(file)
	LoadLocalUsed(file)
	LoadReport(file)



}



func LoadMonitor(file *ini.File) {
	PsMonitorIsUp = file.Section("monitor").Key("process").MustBool(true)
	FileMonitorIsUp = file.Section("monitor").Key("file").MustBool(true)
	NetMonitorIsUp = file.Section("monitor").Key("network").MustBool(false)
}

func LoadLocalUsed(file *ini.File) {
	PsUsedPort = file.Section("monitor").Key("psUsedPort").MustInt(65431)
	FileUsedPort = file.Section("monitor").Key("fileUsedPort").MustInt(65432)
	NetUsedPort = file.Section("monitor").Key("netUsedPort").MustInt(65433)
}

func LoadReport(file *ini.File) {
	ReportType = file.Section("report").Key("type").MustString("https")
	ReportHost = file.Section("report").Key("host").MustString("127.0.0.1")
	ReportPort = file.Section("report").Key("port").MustInt(8080)
}