package filter

import (
	"akagent/setting"
	"akagent/utils"
	"fmt"
	"path"
	"reflect"
	"regexp"
	"runtime"
)

type Rule struct {
	Type string `json:"type"`
	Data string `json:"data"`
}

type Filter struct {
	Meta struct {
		Name        string `json:"name"`        // 名称
		Author      string `json:"author"`      // 编写人
		Description string `json:"description"` // 描述
	} `json:"meta"`              // 规则信息
	Source string            `json:"source"` // 选择判断来源
	System string            `json:"system"` // 匹配系统
	Rules  map[string]Rule `json:"rules"`  // 具体匹配规则
	Enabled	bool			`json:"enabled"`	//开启状态
	And    bool              `json:"and"`    // 规则逻辑
}

var FilterMap  map[string]map[string][]Filter

func FilterInit()  {
	FilterMap = make(map[string]map[string][]Filter)

	FilterMap[runtime.GOOS] = make(map[string][]Filter)

	loadFilter(setting.PrivateFilter)
	loadFilter(setting.PublicFilter)

	fmt.Println("文件事件相关白名单规则数量:", len(FilterMap[runtime.GOOS]["file"]))
}

func loadFilter(filterFile string){
	JsonParse := utils.NewJsonStruct()
	filterlist := []Filter{}

	JsonParse.Load(filterFile, &filterlist)

	for _,v := range filterlist {
		if v.System == runtime.GOOS {
			FilterMap[runtime.GOOS][v.Source] = append(FilterMap[runtime.GOOS][v.Source], v)
		}
	}
}

//Match true= drop
func (f *Filter)Match(val interface{}) bool {
	if !f.Enabled {
		return false
	}

	return f.filterMatch(val)
}

//filterMatch filter匹配 返回true= drop
func (f *Filter)filterMatch(val interface{}) bool {
	var isValid bool

	item := reflect.ValueOf(val).Elem()
	itemType := item.Type()

	isValid = f.And

	// Iterate over the struct fileds
	for i := 0; i < item.NumField(); i++ {
		field := item.Field(i)
		//fmt.Println(field,itemType.Field(i).Tag.Get("json"))
		filterValue, ok := f.Rules[itemType.Field(i).Tag.Get("json")]
		if ok {
			if f.And {
				switch filterValue.Type {
				case "string":
					if filterValue.Data != field.Interface() {
						isValid = false
						break
					}
				case "path-ext":
					fileSuffix := path.Ext(field.Interface().(string))
					if filterValue.Data != fileSuffix {
						isValid = false
						break
					}
				case "regex":
					if ok, _ := regexp.Match(filterValue.Data, []byte(field.Interface().(string))); !ok {
						isValid = false
						break
					}
				}
			}else{
				switch filterValue.Type {
				case "string":
					if filterValue.Data == field.Interface() {
						isValid = true
						break
					}
				case "path-ext":
					fileSuffix := path.Ext(field.Interface().(string))
					if filterValue.Data == fileSuffix {
						isValid = true
						break
					}
				case "regex":
					if ok, _ := regexp.Match(filterValue.Data, []byte(field.Interface().(string))); ok {
						isValid = true
						break
					}
				}
			}
		}
	}

	return isValid
}