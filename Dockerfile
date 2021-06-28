FROM golang:1.16 AS development
ENV GO111MODULE=on \
    GOPROXY=https://goproxy.cn,direct

WORKDIR $GOPATH/src
COPY . .
RUN go build --tags netgo -o akagent

FROM golang:1.16 AS production
WORKDIR /root/
COPY config.json    .
COPY private.filter    .
COPY public.filter    .
COPY --from=development /go/src/akagent .
ENTRYPOINT ["./akagent","config.json"]