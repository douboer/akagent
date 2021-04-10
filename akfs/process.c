#include "akfs.h"
#include "process.h"

#define PsUsedPort  65431

int process_send2Go(unsigned char *buffer ,unsigned int size){
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		return -1;
	}

    struct sockaddr_in sockaddrin;
	memset(&sockaddrin, 0, sizeof(sockaddrin));
	sockaddrin.sin_family = AF_INET;
	sockaddrin.sin_port = htons(PsUsedPort);
	sockaddrin.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sock, (char *)buffer, size, 0, (struct sockaddr *)&sockaddrin, sizeof(sockaddrin));
    close(sock);
	return 0;
}

/**
 * @brief process_callback
 *   回调处理模块
 */
static int process_callback(unsigned char *buffer ,unsigned int size)
{
    process_send2Go(buffer,size);
//
//    akfs_process_t *p = NULL;
//
//    p = (akfs_process_t *)buffer;
//    printf("type:[%d] pid %d ppid %d tgid %d tpath:[%s] ns:[%u] args:[%s] hash:[%s] timestamp[%s]\n" ,p->data_type ,p->pid ,p->ppid ,p->gid ,
//        p->exec_file ,p->ns ,p->argv ,p->exec_hash ,p->timestamp);

    return 0;
}

int ProcessMonitor(void)
{
    int ret = 0;
    akfs_t gat;

    ret = akfs_open(&gat ,"/opt/mount/process");
    if(ret){return ret;}

    ret = akfs_get_access(&gat);
    if(ret){
        akfs_close(&gat);
        goto out;
    }

    akfs_loop_read(&gat ,process_callback);

    akfs_close(&gat);
out:
    return 0;
}