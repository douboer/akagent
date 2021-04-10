#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "akfs.h"

static int net_callback(unsigned char *buffer ,unsigned int size)
{
    akfs_net_t *p = NULL;

    p = (akfs_net_t *)buffer;
    printf("exe_file:[%s]\n" ,p->exec_file);

    return 0;
}

int main(void)
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

