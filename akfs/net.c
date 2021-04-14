#include "net.h"
#include "togo.h"

#define NetUsedPort  65433

/**
 * @brief process_callback
 *   回调处理模块
 */
static int net_callback(unsigned char *buffer ,unsigned int size)
{
    send2Go(buffer,size,NetUsedPort);
//
//    akfs_process_t *p = NULL;
//
//    p = (akfs_process_t *)buffer;
//    printf("type:[%d] pid %d ppid %d tgid %d tpath:[%s] ns:[%u] args:[%s] hash:[%s] timestamp[%s]\n" ,p->data_type ,p->pid ,p->ppid ,p->gid ,
//        p->exec_file ,p->ns ,p->argv ,p->exec_hash ,p->timestamp);

    return 0;
}

int NetMonitor(void)
{
    int ret = 0;
    akfs_t gat;

    ret = akfs_open(&gat ,"/opt/mount/net");
    if(ret){return ret;}

    ret = akfs_get_access(&gat);
    if(ret){
        akfs_close(&gat);
        goto out;
    }

    akfs_loop_read(&gat ,net_callback);

    akfs_close(&gat);
out:
    return 0;
}