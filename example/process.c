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
    printf("type:[%d] pid %d tpath:[%s] ns:[%u]\n" ,
            p->type ,p->pid ,p->tpath ,p->ns);

    return 0;
}

int main(void)
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
