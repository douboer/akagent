#ifndef __LOADBALANCE_H__
#define __LOADBALANCE_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(__USE_UNIX__)
#include <sys/epoll.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif defined(__USE_WIN32__)
#endif

#include "config.h"
#include "module.h"
#include "scheduling.h"


/**
 * @brief 事件类型
 */
enum __lb_event_type_s{
    LB_EVENT_INIT   = 1,
    LB_EVENT_ACCEPT = 2,
    LB_EVENT_CLIENT = 4,
};

/**
 * @brief 事件结构体 指定事件的处理函数 
 */
typedef struct lb_event_s{
    int fd;
    int type;
    int (*handle)(void *);
    struct list_head list;
    struct sockaddr_in addr;
}lb_event_t;

/**
 * @brief 网络事件操作函数
 *  add 将套接字添加到网络事件监听
 *  del 从网络事件中删除套接字
 */
typedef struct lb_operation_s{
int (*add)(lb_event_t *lv);
int (*del)(lb_event_t *lv);
}lb_operation_t;

/**
 * @brief lb模块全局结构体
 */
typedef struct lb_global_s{
    void (*lb_wait)(struct lb_global_s *);
    struct list_head post_queue;
    struct list_head accept_queue;
}lb_global_t;

#if defined(__USE_UNIX__)

int __lb_epoll_add(lb_event_t *lv);
#define lb_event_add __lb_epoll_add

int __lb_epoll_del(lb_event_t *lv);
#define lb_event_del __lb_epoll_del

void __lb_epoll_wait(lb_global_t *glb);

#elif defined(__USE_WIN32__)
//待补充
#endif

int lb_process(config_t *gconfig);

#endif
