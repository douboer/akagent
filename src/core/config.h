#ifndef __MANGOSTEEN_CONFIG_H__
#define __MANGOSTEEN_CONFIG_H__

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cjson.h"
#include "assertion.h"
#include "pri.h"
#include "list.h"
#include "rbtree.h"
#include "os.h"

/**
 * @brief 线程至少创建八个
 */
#define CONFIG_MIN_CPUS 8

/**
 * @brief 
 *  p 配置文件路径
 *  cpus 需要开启的线程数
 *  polling 定时任务轮询的时间
 *  sched_operation 定时任务相关操作
 *      任务创建 添加任务
 *  event_operation 事件相关操作
 *      添加一个描述符到事件上
 */
typedef struct config_s{
    char *path;
    cJSON *json_ptr;
    const void *sym_operation;
    const void *sched_operation;
    const void *event_operation;
    unsigned int cpus;
    unsigned int h : 1;
    unsigned int d : 1;
    unsigned char sched_model[32];
}config_t;

/**
 * @brief config_loading 
 *   检查配置文件合法性
 *   解析配置文件
 */
int config_loading(config_t *gconfig);

#endif
