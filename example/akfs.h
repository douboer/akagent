#ifndef __AKFS_H__
#define __AKFS_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <errno.h>

/**
 * @brief akfs ring
 */
typedef struct akfs_ring_s{
    unsigned int offset;
    unsigned int size;
    unsigned int in;
    unsigned int out;
}akfs_ring_t;

#define min(x, y) ({                        \
        typeof(x) _min1 = (x);          \
        typeof(y) _min2 = (y);          \
        (void) (&_min1 == &_min2);      \
        _min1 < _min2 ? _min1 : _min2; })

unsigned int akfs_ring_get(akfs_ring_t *,unsigned char *,unsigned int);

/**
 * @brief akfs内置的IOCTL命令 
 */
#define AKFS_IOCTL_MAGIC 'a'

#define AKFS_IOCTL_MLEN _IO(AKFS_IOCTL_MAGIC ,0)

#define AKFS_IOCTL_OTP _IO(AKFS_IOCTL_MAGIC ,20)

/**
 *@brief otp交互状态 
 */
enum AKFS_FSA_S{
    AKFS_FSA_REQ    = 1,
    AKFS_FSA_ACCESS = 2,
    AKFS_FSA_REPLY  = 4,
};

/**
 *@brief 
 *  otp相关
 */
typedef struct akfs_otp_s{
    unsigned int status;
    unsigned int seed;
}akfs_otp_t;

/**
 * @brief akfs结构体
 */
typedef struct akfs_s akfs_t;

struct akfs_s{
    int fd;
    int msize;
    akfs_otp_t otp;
    akfs_ring_t *ring;
    unsigned int blen;
    unsigned char *buffer;
    void (*close)(akfs_t *);
    int (*access)(akfs_t *);
    int (*read)(akfs_t *);
};

/**
 * @brief akfs进程数据结构体 
 */
typedef struct akfs_process_s{
    unsigned int type;
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    unsigned int ns;
    char tpath[256];
    char ppath[256];
}akfs_process_t;

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

int akfs_open(akfs_t *at ,const char *dev);

/**
 * @brief akfs_close 
 *   akfs close操作接口
 */
void akfs_close(akfs_t *at);

/**
 * @brief akfs_access 
 *   akfs申请权限接口
 *   需要先经过otp校验之后
 *   才可以mmap映射
 */
int akfs_get_access(akfs_t *at);

typedef int (*akfs_loop_func_t)(unsigned char * ,unsigned int);
void akfs_loop_read(akfs_t *at ,akfs_loop_func_t looper);

#endif
