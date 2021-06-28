# akagent

#### 介绍
主机侧用户层代理程序源码，服务获取内核hook数据，进行数据格式化以及策略过滤后发送到服务端。

#### 示例配置(根据需要自行更改)
```json
{
  "process_cfg": {
    "enable": true,
    "use_inner_port": 65431
  },
  "file_cfg": {
    "enable": true,
    "use_inner_port": 65432,
    "monitor_path_map": {
      "/": "default"
    },
    "file_resave": {
      "enable": false,
      "save_path": "/opt/file/monitor/",
      "maxSizeMB": 100
    }
  },
  "net_cfg": {
    "enable": true,
    "use_inner_port": 65433
  },
  "report": {
    "enable": false,
    "type": "https",
    "host": "",
    "port": 8080
  },
  "filter_cfg": {
    "public_filter": "public.filter",
    "private_filter": "private.filter"
  }
}
```