#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "akfs.h"

static int net_callback(unsigned char *buffer ,unsigned int size)
{
    akfs_net_t *net = NULL;

    net = (akfs_net_t *)buffer;

    printf("data_type:%d, pid:%d, gid:%d, uid:%d, ns:%u, sip:%u.%u.%u.%u,dip:%u.%u.%u.%u, sport:%u, dport:%u task:[%s]\n",net->data_type, net->pid, net->gid, net->uid, net->ns, NIPQUAD(net->srcip),NIPQUAD(net->dstip), ntohs(net->src_port), ntohs(net->dst_port) ,net->exec_file);

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

