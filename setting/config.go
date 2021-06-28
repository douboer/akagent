package setting

import (
	"encoding/json"
	"io/ioutil"
)

type Config struct {
	ProcessCfg ProcessCfg `json:"process_cfg"`
	FileCfg    FileCfg    `json:"file_cfg"`
	NetCfg     NetCfg     `json:"net_cfg"`
	Report     Report     `json:"report"`
	FilterCfg  FilterCfg  `json:"filter_cfg"`
}

//ProcessCfg 网络监控配置
type ProcessCfg struct {
	Enable       bool `json:"enable"`
	UseInnerPort int  `json:"use_inner_port"`
}

//FileCfg 网络监控配置
type FileCfg struct {
	Enable         bool                   `json:"enable"`
	UseInnerPort   int                    `json:"use_inner_port"`
	MonitorPathMap map[string]interface{} `json:"monitor_path_map"` // 指定监控路径，默认全监控
	FileReSave     FileReSave             `json:"file_resave"`
}

//NetCfg 网络监控配置
type NetCfg struct {
	Enable       bool `json:"enable"`
	UseInnerPort int  `json:"use_inner_port"`
}

//FileReSave 文件另存配置
type FileReSave struct {
	Enable    bool   `json:"enable"`    //开关
	SavePath  string `json:"save_path"` //文件存储路径，该路径变动不监控
	MaxSizeMB int    `json:"maxSizeMB"` //最大存储文件大小，单位MB
}

//Report 事件上报配置
type Report struct {
	Enable bool   `json:"enable"` //开关
	Type   string `json:"type"`   //上报方式 https or http
	Host   string `json:"host"`
	Port   int    `json:"port"`
}

type FilterCfg struct {
	PublicFilter  string `json:"public_filter,omitempty"`
	PrivateFilter string `json:"private_filter,omitempty"`
}

func (cfg *Config) DefaultConfig() {
	cfg.ProcessCfg = ProcessCfg{
		Enable:       true,
		UseInnerPort: 65431,
	}

	cfg.NetCfg = NetCfg{
		Enable:       true,
		UseInnerPort: 65433,
	}

	cfg.FileCfg = FileCfg{
		Enable:       true,
		UseInnerPort: 65432,
	}

	if cfg.FileCfg.MonitorPathMap == nil {
		cfg.FileCfg.MonitorPathMap = make(map[string]interface{})
		cfg.FileCfg.MonitorPathMap["/"] = "default"
	}

	cfg.FileCfg.FileReSave = FileReSave{
		Enable:    true,
		SavePath:  "/opt/file/monitor/",
		MaxSizeMB: 100,
	}

	cfg.Report = Report{
		Enable: false,
		Type:   "https",
		Host:   "",
		Port:   8080,
	}

	cfg.FilterCfg = FilterCfg{
		PrivateFilter: "private.filter",
		PublicFilter: "public.filter",
	}
}

func (cfg *Config) Load(configFile string) error {
	JsonParse := NewJsonStruct()
	filterlist := cfg

	return JsonParse.Load(configFile, &filterlist)
}

type JsonStruct struct {
}

func NewJsonStruct() *JsonStruct {
	return &JsonStruct{}
}

func (jst *JsonStruct) Load(filename string, v interface{}) error {
	//ReadFile函数会读取文件的全部内容，并将结果以[]byte类型返回
	data, err := ioutil.ReadFile(filename)
	if err != nil {
		return err
	}

	//读取的数据为json格式，需要进行解码
	err = json.Unmarshal(data, v)
	if err != nil {
		return err
	}
	return nil
}
