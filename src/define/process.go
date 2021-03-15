package define

//ProcessMonitor 进程监控接口字段
type ProcessMonitor struct {
	Exe_file  string `json:"exe_file"`	//进程文件
	Exe_hash  string `json:"exe_hash"`	//文件hash
	Pid       uint32 `json:"pid"`		//进程id
	Ppid      uint32 `json:"ppid"`		//父进程ID
	Data_type uint32 `json:"data_type" enum:"1001,1002,1003"`	//数据类型
	Argv      string `json:"argv"`		//进程参数
	Uid       uint32 `json:"uid"`		//用户ID
	Gid       uint32 `json:"gid"`		//用户组ID
	Namespace uint32 `json:"namespace"`	//进程命名空间ID
	Timestamp uint64 `json:"timestamp"`	//事件时间戳
}
