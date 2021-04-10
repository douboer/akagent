#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "akfs.h"

static int process_callback(unsigned char *buffer ,unsigned int size)
{
    akfs_process_t *p = NULL;

    p = (akfs_process_t *)buffer;
    printf("type:[%d] pid %d ppid %d tgid %d parent:[%s] tgid %d tpath:[%s] ns:[%u] args:[%s] hash:[%s] timestamp[%ld]\n" ,p->data_type ,p->pid ,p->ppid ,p->ptgid ,p->parent_name,p->gid ,p->exec_file ,p->ns ,p->argv ,p->exec_hash ,p->timestamp);

    return 0;
}

int main(void)
{
    int ret = 0;
    akfs_t gat;

    ret = akfs_open(&gat ,"/opt/mount/process");
    if(ret){return ret;} 

    ret = ioctl(gat.fd ,AKFS_IOCTL_POLICY ,"hello world 123");
    printf("before AKFS_IOCTL_POLICY ret %d\n" ,ret);


    ret = akfs_get_access(&gat);
    if(ret){
        akfs_close(&gat);
        goto out;
    }

    ret = ioctl(gat.fd ,AKFS_IOCTL_POLICY ,"hello world 123");
    printf("after AKFS_IOCTL_POLICY ret %d\n" ,ret);


    akfs_loop_read(&gat ,process_callback);

    akfs_close(&gat);
out:
    return 0;
}

