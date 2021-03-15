#include "akfs.h"

/**
 * @brief akfs_ring_get
 *   从ring里面读取数据，多线程需要自己加锁
 */
unsigned int akfs_ring_get(akfs_ring_t *ring ,unsigned char *data ,unsigned int len)
{
    unsigned int l;
    unsigned char *buffer = (unsigned char *)ring + ring->offset;

    len = min(len, ring->in - ring->out);

    l = min(len ,ring->size - (ring->out & (ring->size - 1)));
    memcpy(data ,buffer + (ring->out & (ring->size - 1)) ,l);

    memcpy(data + l ,buffer,len - l);

    ring->out += len;

    return len;
}

/**
 * @brief akfs_def_close
 *   akfs 关闭接口 释放资源
 */
static void akfs_def_close(akfs_t *at){
    if(at->buffer){
        free(at->buffer);
    }

    if(at->ring){
        munmap(at->ring ,at->msize);
    }

    close(at->fd);
}

/**
 * @brief akfs_def_get_msize
 *   获取mmap需要映射的长度
 */
static int akfs_def_get_msize(akfs_t *at){
    return ioctl(at->fd ,AKFS_IOCTL_MLEN ,&at->msize);
}

/**
 * @brief akfs_def_access
 *   otp 校验接口
 *   可根据需要调整算法，需要和akfs对应接口一致即可
 */
static int akfs_def_access(akfs_t *at){
    return 0;
}

static int akfs_def_read(akfs_t *at){
    return akfs_ring_get(at->ring ,at->buffer ,at->blen);
}

/**
 * @brief akfs_open
 *   akfs open操作函数
 *   初始化akfs资源
 */
int akfs_open(akfs_t *at ,const char *dev)
{
    int ret = 0;

    at->fd = open(dev ,O_RDWR ,0644);
    if(at->fd < 0){
        return -errno;
    }

    ret = akfs_def_get_msize(at);
    if(ret){
        goto out;
    }

    //blen后期可以改成通过ioctl获取大小
    at->blen = PAGE_SIZE;
    at->buffer = (unsigned char *)malloc(at->blen);
    if(!at->buffer){
        goto out;
    }

    at->close = akfs_def_close;
    at->access = akfs_def_access;
    at->read = akfs_def_read;

    return 0;

out:
    akfs_def_close(at);
    return -EACCES;
}

/**
 * @brief akfs_close
 *   akfs close操作接口
 */
void akfs_close(akfs_t *at){
    at->close(at);
}

/**
 * @brief akfs_access
 *   akfs申请权限接口
 *   需要先经过otp校验之后
 *   才可以mmap映射
 */
int akfs_get_access(akfs_t *at)
{
    int ret =0;

    ret = at->access(at);
    if(ret){
        return ret;
    }

    at->ring = (akfs_ring_t *)mmap(NULL ,at->msize ,PROT_READ|PROT_WRITE ,MAP_SHARED ,at->fd ,0);
    if(at->ring == MAP_FAILED){
        return -errno;
    }

    return 0;
}

void akfs_loop_read(akfs_t *at ,akfs_loop_func_t looper)
{
    unsigned int size = 0;
    int ret = 0;
    struct pollfd pd;

    pd.fd = at->fd;
    pd.events = POLLIN | POLLERR;

    do{
        do{
            size = at->read(at);;
            if(!size){
                break;
            }

            ret = looper ? looper(at->buffer ,size) : 0;
            if(ret){
                return;
            }

       }while(1);
        ret = poll(&pd ,2 ,-1);
    }while(1);
}
