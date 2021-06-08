package main

import (
	"akagent/src/filter"
	"akagent/src/monitor"
)

func main() {
	//加载过滤规则集
	filter.FilterInit()

	monitor.MonitorStart()
	select {
	}
}