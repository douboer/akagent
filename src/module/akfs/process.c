#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "module.h"
#include "akfs.h"

#define MODULE_PRI_PROCESS_INIT (MODULE_PRI_DEF + 1)

static int process_init(config_t *gconfig);
static int process_check(config_t *gconfig);

void process_task_run(config_t *gconfig);

int process_test(struct sched_task_s *task);

/**
 * @brief process模块
 */
static module_t gprocess = {
    .name = "process" ,
    .check = process_check,
    .init= process_init,
};

/**
 * @brief PRE_DEFINE 
 * process模块预初始化
 * 将process初始化挂载到模块队列上
 */
static PRE_DEFINE(MODULE_PRI_PROCESS_INIT) void process_pre_init(void){
    module_register(&gprocess);
}

static int fd = 0;
static akfs_ring_t *ring = NULL;

unsigned long len;

/**
 * @brief process_check 
 * process检测模块
 */
static int process_check(config_t *gconfig)
{
    fd=open("/opt/mount/process" ,O_RDWR ,0644);
    assert_error(fd > 0 ,-EACCES);

    ioctl(fd ,AKFS_IOCTL_MLEN ,&len);

    ring = (akfs_ring_t *)mmap( NULL ,len ,PROT_READ|PROT_WRITE ,MAP_SHARED ,fd ,0);
    if(ring == MAP_FAILED){
        perror("mmap failed\n");
        goto out;
    }

    return 0;
out:
    close(fd);
    return -1;
}
/**
 * @brief process_init 
 *   process 初始化模块
 */
static int process_init(config_t *gconfig)
{
    process_task_run(gconfig);
    return 0;
}

int process_test(struct sched_task_s *task)
{
    unsigned int size = 0;
    akfs_process_t *p= NULL;
    unsigned char buffer[PAGE_SIZE] = {0};

    printf("PAGE_SIZE is %d\n" ,PAGE_SIZE);

    do{
        sleep(1);
        do{
            size = __akfs_ring_get(ring ,buffer ,PAGE_SIZE);
            if(!size){
                printf("__akfs_ring_get %u\n" ,size);
                break;
            }

            p = (akfs_process_t *)buffer;
            printf("type:[%d] pid %d tpath:[%s] ns:[%u]\n" ,
                    p->type ,p->pid ,p->tpath ,p->ns);
        }while(1);
    }while(1);

    return TASK_RUNNING;
}

void process_task_run(config_t *gconfig)
{
    sched_task_t *task = NULL;

    task = sched_task_alloc();

    task->name = "process";

    task->handle = process_test;

    task->polling = 500;

    sched_task_register(task);
}
