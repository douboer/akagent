package main

import (
	"akagent/setting"
	"akagent/src/filter"
	"akagent/src/monitor"
	log "github.com/sirupsen/logrus"
	"os"
)


func main() {

	formatter := new(log.TextFormatter)
	formatter.FullTimestamp = true
	formatter.TimestampFormat = "2006-01-02 15:04:05"
	log.SetFormatter(formatter)
	log.SetOutput(os.Stdout)

	var cfg = setting.Config{
	}
	//cfg.DefaultConfig()
	//	data, _ := json.Marshal(&cfg)
	//	log.Infof("默认配置:%s",string(data))

	if len(os.Args) < 2  {
		log.Fatalf("请指定配置文件. \n 示例: ./akagent config.json")
	}

	err := cfg.Load(os.Args[1])
	if err != nil {
		log.Fatalf("加载配置文件出错: %s",err.Error())
	}

	//加载过滤规则集
	filter.FilterInit(cfg)

	monitor.MonitorStart(cfg)
	select {
	}
}