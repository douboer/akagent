#include "process.h"

#define MODULE_PRI_PROCESS_INIT (MODULE_PRI_DEF + 1)

static int process_init(config_t *gconfig);
static int process_check(config_t *gconfig);
static int process_task_register(config_t *gconfig);

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
static lb_event_t glv;

/**
 * @brief process_check 
 * process检测模块
 */
static int process_check(config_t *gconfig)
{
    return 0;
}

/**
 * @brief process_init 
 *   process 初始化模块
 */
static int process_init(config_t *gconfig)
{
    assert_error(!process_task_register(gconfig) ,-EACCES);

    return 0;
}

static int process_event_handle(void *data)
{
    unsigned int size = 0;
    akfs_process_t *p = NULL;
    sched_task_t *task = (sched_task_t *)data;

    printf("process_event_handle..\n");

    do{
        size = gat.read(&gat);
        assert_break(size > 0 ,); 

        p = (akfs_process_t *)gat.buffer;
        printf("type:[%d] pid %d ppid %d gid %d tpath:[%s] ns:[%u] args:[%s] hash:[%s] timestamp[%ld]\n" ,p->data_type ,p->pid ,p->ppid ,p->gid ,
                p->exec_file ,p->ns ,p->argv ,p->exec_hash ,p->timestamp);

    }while(1);

    lb_event_add(task->reserve);

    return TASK_EXIT;
}

static int process_handle(void *task)
{
    int status = TASK_RUNNING ,ret = 0;
    
    printf("process_handle....\n");

    ret = akfs_open(&gat ,"/opt/mount/process");
    assert_goto(!ret ,out ,);

    ret = akfs_get_access(&gat);
    assert_goto(!ret ,out ,akfs_close(&gat););

    glv.fd = gat.fd;
    glv.type = LB_EVENT_CLIENT;
    glv.handle = process_event_handle;

    ret = lb_event_add(&glv);
    printf("lb_event_add ret %d\n" ,ret);

    status = TASK_EXIT;

out:
    return status;
}

/**
 * @brief process_task_register 
 *   初始化的时候注册一个任务
 *   当和akfs交互成功之后任务退出
 *   从ring里面读取数据
 */
static int process_task_register(config_t *gconfig)
{
    sched_task_t *task = NULL;

    task = sched_task_alloc();

    task->name = "process";

    task->handle = process_handle;

    task->polling = 500;

    sched_task_register(task);

    return 0;
}
