package report

import (
	"bytes"
	"crypto/tls"
	"io/ioutil"
	"net"
	"net/http"
	"time"
)

type HttpReport struct {
	Client	http.Client
	TargetUrl string
	Content   []byte
}

var HTTPTransport = &http.Transport{
	DialContext: (&net.Dialer{
		Timeout:   30 * time.Second, // 连接超时时间
		KeepAlive: 60 * time.Second, // 保持长连接的时间
	}).DialContext, // 设置连接的参数
	MaxIdleConns:          500, // 最大空闲连接
	IdleConnTimeout:       60 * time.Second, // 空闲连接的超时时间
	ExpectContinueTimeout: 30 * time.Second, // 等待服务第一个响应的超时时间
	MaxIdleConnsPerHost:   100, // 每个host保持的空闲连接数
	TLSClientConfig: &tls.Config{
		InsecureSkipVerify: true,
		MinVersion:tls.VersionTLS12,
		PreferServerCipherSuites:true,
	},
}

func NewHttpReport() *HttpReport {
	return &HttpReport{
		Client: http.Client{Transport: HTTPTransport},
	}
}

func (h *HttpReport)Post() error {
	// 判断 WebHook 通知
	reader := bytes.NewReader(h.Content)
	req, err := http.NewRequest(http.MethodPost, h.TargetUrl, reader)
	req.Header.Set("Content-Type", "application/json;charset=UTF-8")
	if err != nil {
		panic("Http Req Failed " + err.Error())
	}
	resp, err := h.Client.Do(req)
	if err != nil {
		return err
	}
	defer resp.Body.Close()
	ioutil.ReadAll(resp.Body) // 如果不及时从请求中获取结果，此连接会占用，其他请求服务复用连接
	return nil
}
