#ifndef __MANGOSTENN_SYMBOL_H__
#define __MANGOSTENN_SYMBOL_H__

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include "module.h"
#include "rbtree.h"
#include "list.h"
#include "assertion.h"
#include "strings.h"
#include "pri.h"

/**
 * @brief 命名空间结构体 
 *  qlist存放自己的子节点
 */
typedef struct sym_queue_s{
    struct list_head list;
    struct list_head qlist;
    void *handle;
    char name[32];
}sym_queue_t;

/**
 * @brief 回调函数节点结构体
 */
typedef struct sym_node_s{
    rbnode_t rbnode;
    struct list_head list;
    void *function;
}sym_node_t;

/**
 * @brief sym支持的操作函数
 *  reg_namespace 注册命名空间
 *  unreg_namespace 卸载命名空间 卸载的时候会清除所有的相关的回调函数
 *  get_namespace 根据命名空间名字获取对应的结构体
 *  set_func 将函数挂载到命名空间下
 *  get_func 根据函数key值获取对应的回调函数
 */
typedef struct sym_operation_s{
    int (*reg_namespace)(sym_queue_t *q);
    int (*unreg_namespace)(const char *n);
    sym_queue_t *(*get_namespace)(const char *name);
    int (*set_func)(sym_queue_t *queue ,sym_node_t *node);
    void *(*get_func)(unsigned char *name);
}sym_operation_t;

/**
 * @brief 回调函数表
 */
typedef struct sym_table_s{
    struct list_head queue;
    rbroot_t tree;
    sym_operation_t ops;
    pthread_mutex_t qlock;
    pthread_mutex_t tlock;
}sym_table_t;


int sym_register(sym_queue_t *q);

int sym_unregister(const char *n);

sym_queue_t *sym_queue_get(const char *name);

int sym_func_set(sym_queue_t *queue ,sym_node_t *node);

void *sym_func_get(unsigned char *name);

#endif
