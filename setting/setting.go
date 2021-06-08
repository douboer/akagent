package setting

import (
	"fmt"
	"gopkg.in/ini.v1"
	"log"
	"os"
	"path/filepath"
)

var (
	ThisPpid uint32
	ProcessAbsexefile string

	PsMonitorIsUp   bool
	FileMonitorIsUp bool
	NetMonitorIsUp  bool

	ReportType string
	ReportHost string
	ReportPort int

	PsUsedPort   int
	FileUsedPort int
	NetUsedPort  int
	ReportEnable bool

	//事件过滤规则文件
	PrivateFilter	string
	PublicFilter	string
)

func init() {
	if _, err := os.Stat("config.ini"); err != nil {
		err := os.MkdirAll("config.ini", 0711)

		if err != nil {
			log.Println(err)
			return
		}
	}

	file, err := ini.Load("config.ini")
	if err != nil {
		fmt.Println("配置文件读取错误，请检查文件路径:", err)
	}

	ThisPpid = uint32(os.Getppid())

	fmt.Println(ThisPpid)
	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	ProcessAbsexefile = fmt.Sprintf("%s/%s",dir,filepath.Base(os.Args[0]))

	LoadMonitor(file)
	LoadLocalUsed(file)
	LoadReport(file)
	LoadFilter(file)
}

func LoadMonitor(file *ini.File) {
	PsMonitorIsUp = file.Section("monitor").Key("process").MustBool(true)
	FileMonitorIsUp = file.Section("monitor").Key("file").MustBool(true)
	NetMonitorIsUp = file.Section("monitor").Key("network").MustBool(true)
}

func LoadLocalUsed(file *ini.File) {
	PsUsedPort = file.Section("monitor").Key("psUsedPort").MustInt(65431)
	FileUsedPort = file.Section("monitor").Key("fileUsedPort").MustInt(65432)
	NetUsedPort = file.Section("monitor").Key("netUsedPort").MustInt(65433)
}

func LoadReport(file *ini.File) {
	ReportEnable = file.Section("report").Key("enable").MustBool(false)
	ReportType = file.Section("report").Key("type").MustString("https")
	ReportHost = file.Section("report").Key("host").MustString("127.0.0.1")
	ReportPort = file.Section("report").Key("port").MustInt(8080)
}

func LoadFilter(file *ini.File) {
	PrivateFilter = file.Section("filter").Key("private").MustString("private.filter")
	PublicFilter = file.Section("filter").Key("public").MustString("public.filter")

	if _, err := os.Stat(PrivateFilter); err != nil {
		err := os.MkdirAll(PrivateFilter, 0711)

		if err != nil {
			log.Println(err)
			return
		}
	}

	if _, err := os.Stat(PublicFilter); err != nil {
		err := os.MkdirAll(PublicFilter, 0711)

		if err != nil {
			log.Println(err)
			return
		}
	}
}