#include "event.h"

int monitorStart(unsigned char *monitor_file ,akfs_loop_func_t looper)
{
    int ret = 0;
    akfs_t gat;

    ret = akfs_open(&gat ,monitor_file);
    if(ret){return ret;}

    ret = akfs_get_access(&gat);
    if(ret){
        akfs_close(&gat);
        goto out;
    }

    akfs_loop_read(&gat ,looper);

    akfs_close(&gat);
out:
    return 0;
}

/**
 * @brief send_2_go_server
 *   发送数据到go处理模块
 */
int send_2_go_server(unsigned char *buffer ,unsigned int size,unsigned int usedPort){
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) {
        return -1;
    }

    struct sockaddr_in sockaddrin;
    memset(&sockaddrin, 0, sizeof(sockaddrin));
    sockaddrin.sin_family = AF_INET;
    sockaddrin.sin_port = htons(usedPort);
    sockaddrin.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sock, (char *)buffer, size, 0, (struct sockaddr *)&sockaddrin, sizeof(sockaddrin));
    close(sock);
    return 0;
}


/**
 * @brief file_callback
 *   文件监控事件回调处理模块
 */
static int file_callback(unsigned char *buffer ,unsigned int size)
{
    return send_2_go_server(buffer,size,file_used_port);
}

int FileMonitor(void){
    monitorStart("/opt/mount/file",file_callback);
}

/**
 * @brief process_callback
 *   进程监控事件回调处理模块
 */
static int process_callback(unsigned char *buffer ,unsigned int size)
{
    return send_2_go_server(buffer,size,process_used_port);
}

int ProcessMonitor(void){
    monitorStart("/opt/mount/process",process_callback);
}

/**
 * @brief net_callback
 *   网络监控事件回调处理模块
 */
static int net_callback(unsigned char *buffer ,unsigned int size)
{
    return send_2_go_server(buffer,size,net_used_port);
}

int NetMonitor(void){
    monitorStart("/opt/mount/net",net_callback);
}