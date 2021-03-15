#include "scheduling.h"

/**
 * @brief 全局定时任务表
 */
static sched_rt_t gsched; 

/**
 * @brief 内置函数
 */
static int __sched_init(config_t *gconfig);

static int __sched_config_parse(config_t *gconfig);
static int __sched_thread_init(config_t *gconfig);

static int __sched_process(config_t *gconfig);
static void *__sched_master_process(void *args);
static void *__sched_worker_process(void *args);

static sched_task_t *__sched_task_pop(void);
static void __sched_worker_wakeup(void);
static void __sched_worker_wait(void);
static void __sched_task_refresh(sched_task_t *task);

/**
 * @brief sched的初始化模块 
 */
static module_t gsched_module = {
    .name = "sched" ,
    .init = __sched_init,
    .check = NULL ,
};

/**
 * @brief __sched_init 
 *   初始化资源预调用
 */
static PRE_DEFINE(TIMING_PRI_INIT) void __sched_pre_init(config_t *gconfig)
{
    pthread_mutex_init(&gsched.qlock ,NULL);

    pthread_mutex_init(&gsched.tlock ,NULL);

    pthread_mutex_init(&gsched.wlock ,NULL);

    pthread_cond_init(&gsched.condition ,NULL);

    list_head_init(&gsched.queue);

    rbtree_init(&gsched.tree ,NULL ,NULL);

    //初始化调度模型
    gsched.get = sched_fair_get;

    gsched.put = sched_fair_put;

    module_register(&gsched_module);
}

/**
 * @brief sched_init 
 *   模块初始化的时候调用
 */
static int __sched_init(config_t *gconfig)
{
    //解析配置文件
    assert_error(!__sched_config_parse(gconfig) ,-EACCES);

    assert_error(!__sched_thread_init(gconfig) ,-ENOMEM);

    //创建master/work线程
    assert_error(!__sched_process(gconfig),-EACCES); 

    return 0;
}

/**
 * @brief __shced_config_parse 
 *   解析配置文件sched相关选项
 */
static int __sched_config_parse(config_t *gconfig)
{
    cJSON *root = NULL ,*sub = NULL;

    root = cJSON_GetObjectItem(gconfig->json_ptr ,"base");
    assert_error(root ,-EACCES);

    sub = cJSON_GetObjectItem(root ,"polling");
    assert_error(sub ,-EACCES);

    gsched.polling = sub->valueint;
    printf("__sched_config_parse %d\n" ,gsched.polling);

    return 0;
}

/**
 * @brief __sched_tstatus_check 
 *   检查任务的状态
 */
#define __sched_tstatus_check(status ,task)   \
    if(status & TASK_EXIT){ \
        sched_task_free(task); \
        continue;   \
    }else if(status & TASK_STOLEN){ \
        continue;   \
    }   

/**
 * @brief __sched_thread_init 
 *   初始化sched的线程池
 */
static inline int __sched_thread_init(config_t *gconfig)
{
    unsigned int cpus;

    cpus = get_sys_cpus();
    gconfig->cpus = (cpus > CONFIG_MIN_CPUS) ? (2 * cpus) : CONFIG_MIN_CPUS;

    gsched.tarray = (pthread_t *)malloc(gconfig->cpus * sizeof(pthread_t));
    assert_error(gsched.tarray ,-ENOMEM);

    return 0;
}

/**
 * @brief sched_process 
 *   创建master/worker线程
 */
static int __sched_process(config_t *gconfig)
{
    int i ,ret;

    //master线程度
    ret = pthread_create(&gsched.tarray[0] ,NULL ,__sched_master_process ,NULL);
    assert_error(!ret ,ret);

    //worker
    for(i = 1 ;i < gconfig->cpus ;i++){
        ret = pthread_create(&gsched.tarray[i] ,NULL ,__sched_worker_process ,NULL);
        assert_error(!ret ,ret);
    }

    return 0;
}

/**
 * @brief __sched_polling_time 
 *   master线程轮询时间
 */
static inline void __sched_polling_time(void){
    use_msleep(gsched.polling);
}

/**
 * @brief __sched_master_process 
 *      master管理线程
 *      负责将任务从tree分发到worker上
 */
static void *__sched_master_process(void *args)
{
    sched_task_t *task = NULL;
    int need = 0;

    pthread_detach(pthread_self());

    while(1){
        __sched_polling_time();

        pthread_testcancel();

        do{
            task = __sched_task_pop();
            assert_break(task ,);

            //将任务丢到执行队列
            gsched.put(&gsched ,task);

            need = 1;
        }while(1);

        //有任务则唤醒worker线程
        assert_void(!need ,need = 0;__sched_worker_wakeup(););
    }

    pthread_exit("exit");
}

/**
 * @brief __sched_worker_process 
 *   worker工作线程
 *   执行超时任务
 */
static void *__sched_worker_process(void *args)
{
    sched_task_t *task = NULL;
    unsigned int status = 0;

    pthread_detach(pthread_self());

    while(1){
        __sched_worker_wait();
        //获取任务
        printf("__sched_worker_process is wakeup\n");
        task = gsched.get(&gsched);
        assert_continue(task ,);

        status = task->handle(task);

        //检查任务状态
        __sched_tstatus_check(status ,task);

        //ready状态的丢回定时任务表
        sched_task_push(task);
    }

    pthread_exit("exit");
}

/**
 * @brief __sched_worker_wakeup 
 *   worker线程唤醒
 */
static void __sched_worker_wakeup(void)
{
    pthread_mutex_lock(&gsched.wlock);

    pthread_cond_broadcast(&gsched.condition);

    pthread_mutex_unlock(&gsched.wlock);
}

/**
 * @brief __sched_worker_wait 
 *   worker线程等待
 */
static void __sched_worker_wait(void)
{
    pthread_mutex_lock(&gsched.wlock);

    pthread_cond_wait(&gsched.condition ,&gsched.wlock);

    pthread_mutex_unlock(&gsched.wlock);
}

/**
 * @brief __sched_task_pop 
 *   从定时任务表获取一个任务
 */
static sched_task_t *__sched_task_pop(void)
{
    sched_task_t *task = NULL;
    unsigned int current;

    current = get_sys_tick();

    pthread_mutex_lock(&gsched.tlock);

    task = (sched_task_t *)rb_first(&gsched.tree.root); 
    assert_goto(task ,out ,);

    //超时，任务该执行了
    if(current > *((int *)task->rbnode.key)){
        rb_erase(&task->rbnode.rbnode ,&gsched.tree.root);
    }else{
        task = NULL;
    }

out:
    pthread_mutex_unlock(&gsched.tlock);

    return task;
}

/**
 * @brief __sched_task_push 
 *   表定时任务加入到定时任务表
 */
int sched_task_push(sched_task_t *task)
{
    __sched_task_refresh(task);

    pthread_mutex_lock(&gsched.tlock);

    rbtree_push(&gsched.tree ,&task->rbnode);

    pthread_mutex_unlock(&gsched.tlock);

    return 0;
}

/**
 * @brief __sched_task_refresh 
 *   刷新task的超时时间
 */
static void __sched_task_refresh(sched_task_t *task){
    *((int *)task->rbnode.key) = get_sys_tick() + task->polling;
}

/**
 * @brief sched_task_alloc 
 *  申请一个task
 */
sched_task_t *sched_task_alloc(void)
{
    sched_task_t *task = NULL;

    task = (sched_task_t *)malloc(sizeof(sched_task_t));
    assert_error(task ,NULL);
   
    return task;
}

/**
 * @brief sched_task_free 
 *   释放一个task
 */
void sched_task_free(sched_task_t *task)
{
    free(task);
}
