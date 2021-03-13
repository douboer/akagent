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

static akfs_t gat;

/**
 * @brief process_check 
 * process检测模块
 */
static int process_check(config_t *gconfig)
{
    int ret = 0;

    ret = akfs_open(&gat ,"/opt/mount/process");
    assert_error(!ret ,ret);

    ret = akfs_get_access(&gat);
    assert_goto(!ret ,out ,akfs_close(&gat););

out:
    return 0;
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

static int process_callback(unsigned char *buffer ,unsigned int size)
{
    akfs_process_t *p = NULL;

    p = (akfs_process_t *)buffer;
    printf("type:[%d] pid %d tpath:[%s] ns:[%u]\n" ,
            p->type ,p->pid ,p->tpath ,p->ns);

    return 0;
}

int process_test(struct sched_task_s *task)
{
    akfs_loop_read(&gat ,process_callback);

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
