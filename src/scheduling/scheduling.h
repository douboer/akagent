#ifndef __SCHED_H__
#define __SCHED_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "config.h"
#include "module.h"

typedef struct sched_rt_s sched_rt_t;
typedef struct sched_task_s sched_task_t;
typedef struct sched_task_operation_s sched_task_operation_t;

/**
 * @brief 任务运行状态
 */
enum __sched_task_status_s{
    TASK_RUNNING = 1,
    TASK_EXIT = 2,
    TASK_STOLEN = 4,
};

/**
 * @brief 任务支持的操作接口 
 */
struct sched_task_operation_s{
    //创建task
    sched_task_t *(*alloc)(void);
    //释放task
    void (*free)(sched_task_t *);
    //添加到定时任务表
    int (*reg)(sched_task_t *);
};

/**
 * @brief 任务结构体
 *      rbnode.key即为超时时间 current + polling
 *      polling为每次任务轮询时间(ms)
 */
struct sched_task_s{
    rbnode_t rbnode;
    struct list_head list;
    const char *name;
    int (*handle)(struct sched_task_s *task);
    unsigned int polling;
    void *reserve;
};

/**
 * @brief 调度全局结构体
 *   维护任务以及分发
 */
struct sched_rt_s{
    rbroot_t tree;
    struct list_head queue;
    pthread_mutex_t tlock;
    pthread_mutex_t qlock;
    pthread_mutex_t wlock;
    pthread_cond_t condition; 
    pthread_t *tarray;
    int (*put)(sched_rt_t *sched ,sched_task_t *);
    sched_task_t *(*get)(sched_rt_t *gsched);
    unsigned int polling;
};

/**
 * @brief sched_init 
 *   调度初始化
 */
//task注册
int sched_task_push(sched_task_t *task);
#define sched_task_register sched_task_push

int sched_fair_put(sched_rt_t *gsched ,sched_task_t *task);

sched_task_t *sched_fair_get(sched_rt_t *gsched);

sched_task_t *sched_task_alloc(void);

void sched_task_free(sched_task_t *task);

#endif
