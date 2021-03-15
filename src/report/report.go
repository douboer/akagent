package report

import (
	"bytes"
	"crypto/tls"
	"io/ioutil"
	"log"
	"net/http"
	"time"
)

type HttpReport struct {
	TargetUrl string
	Content   []byte
}



func (h *HttpReport)Post() error {
	// 判断 WebHook 通知
	reader := bytes.NewReader(h.Content)

	timeout := time.Duration(3) * time.Second
	tr := &http.Transport{
		TLSClientConfig: &tls.Config{InsecureSkipVerify: true},
	}

	request, _ := http.NewRequest("POST", h.TargetUrl, reader)
	request.Header.Set("Content-Type", "application/json;charset=UTF-8")
	client := &http.Client{
		Transport: tr,
		Timeout:   timeout,
	}
	resp, err := client.Do(request)

	if err != nil {
		log.Print("上报记录失败.", err)
	} else {
		log.Print("上报记录成功.")
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		log.Print(err)
	}
	log.Print("回应：", string(body))
	return nil
}
