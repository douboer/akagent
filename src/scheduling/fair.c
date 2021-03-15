#include "scheduling.h"

/**
 * @brief sched_fair_put 
 *   将任务放到共享执行队列
 */
int sched_fair_put(sched_rt_t *gsched ,sched_task_t *task)
{
    pthread_mutex_lock(&gsched->qlock);

    list_add_tail(&task->list ,&gsched->queue);

    pthread_mutex_unlock(&gsched->qlock);

    return 0;
}

/**
 * @brief sched_fair_get 
 *   从共享执行队列取出任务来
 */
sched_task_t *sched_fair_get(sched_rt_t *gsched)
{
    sched_task_t *task = NULL;

    pthread_mutex_lock(&gsched->qlock);

    if(!list_empty(&gsched->queue)){
        task = list_first_entry(&gsched->queue,sched_task_t ,list);
        assert_goto(!task ,out ,list_del_init(&task->list););
    }

out:
    pthread_mutex_unlock(&gsched->qlock);

    return task;
}
