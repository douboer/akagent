#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "akfs.h"

static int file_callback(unsigned char *buffer ,unsigned int size)
{
    akfs_file_t *p = NULL;

    p = (akfs_file_t *)buffer;
    printf("type:[%d] pid %d ppid %d tgid %d file:[%s] ns:[%u] chg:[%s] timestamp[%ld]\n" ,p->data_type ,p->pid ,p->ppid ,p->ptgid ,p->exec_file ,p->ns ,p->chg_file ,p->timestamp);

    return 0;
}

int main(void)
{
    int ret = 0;
    akfs_t gat;

    ret = akfs_open(&gat ,"/opt/mount/file");
    if(ret){return ret;} 

    ret = akfs_get_access(&gat);
    if(ret){
        akfs_close(&gat);
        goto out;
    }

    akfs_loop_read(&gat ,file_callback);

    akfs_close(&gat);
out:
    return 0;
}

