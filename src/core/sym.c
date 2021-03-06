#include "sym.h"

static sym_table_t gsym;

static sym_queue_t *__sym_queue_del(const char *name);
static void __sym_def_destroy(struct rb_node *node);
static int __sym_def_compare(unsigned char *k1 ,unsigned char *k2);
static int __sym_init(config_t *gconfig);
static void __sym_ops_init(void);

static module_t gsym_module = {
    .name = "sym",
    .init = __sym_init,
    .check = NULL,
};

/**
 * @brief sym_init 
 *   初始化全局函数表
 */
static PRE_DEFINE(SYM_PRT_INIT) void __sym_pre_init(void)
{
    pthread_mutex_init(&gsym.qlock ,NULL);

    pthread_mutex_init(&gsym.tlock ,NULL);

    list_head_init(&gsym.queue);

    rbtree_init(&gsym.tree ,__sym_def_destroy ,__sym_def_compare);

    __sym_ops_init();

    module_register(&gsym_module);
}

/**
* @brief __sym_ops_init 
*   sym初始化操作函数
*/
static void __sym_ops_init(void)
{
    gsym.ops.reg_namespace = sym_register;    
    gsym.ops.unreg_namespace = sym_unregister;
    gsym.ops.get_namespace = sym_queue_get;    
    gsym.ops.set_func = sym_func_set;
    gsym.ops.get_func = sym_func_get; 
}

/**
 * @brief __sym_init 
 *   模块初始化调用
 */
static int __sym_init(config_t *gconfig)
{
    gconfig->sym_operation = &gsym.ops;
    return 0;
}

/**
 * @brief 
 *   添加一个队列节点
 */
int sym_register(sym_queue_t *q)
{
    struct list_head *pos = NULL ,*tmp = NULL;
    sym_queue_t *queue = NULL;
    int ret = 0;

    pthread_mutex_lock(&gsym.qlock);

    //查看是否存在同名节点
    list_for_each_safe(pos ,tmp ,&gsym.queue){
        queue = list_entry(pos ,sym_queue_t ,list);
        assert_break(string_strcmp(queue->name ,q->name),);
        queue = NULL;
    }

    assert_goto(!queue ,out ,ret = -EEXIST);

    list_add(&q->list ,&gsym.queue);

out:
    pthread_mutex_unlock(&gsym.qlock);

    return ret;
}

/**
 * @brief 
 *   删除一个队列节点
 */
int sym_unregister(const char *name)
{
    sym_queue_t *queue = NULL;
    sym_node_t *snode = NULL;
    struct list_head *pos = NULL ,*tmp = NULL;

    queue = __sym_queue_del(name);
    assert_error(queue ,-ENODATA);


    //遍历链表把节点从红黑树上踢出
    pthread_mutex_lock(&gsym.tlock);

    list_for_each_safe(pos ,tmp ,&queue->qlist){
        snode = list_entry(pos ,sym_node_t ,list);
        rb_erase(&snode->rbnode.rbnode ,&gsym.tree.root);
        free(snode);
    }

    pthread_mutex_unlock(&gsym.tlock);

    free(queue);

    return 0;
}

/**
 * @brief 根据名称找到对应的节点
 */
sym_queue_t *sym_queue_get(const char *name)
{
    struct list_head *pos = NULL ,*tmp = NULL;
    sym_queue_t *queue = NULL;

    pthread_mutex_lock(&gsym.qlock);

    list_for_each_safe(pos ,tmp ,&gsym.queue){
        queue = list_entry(pos ,sym_queue_t ,list);
        assert_break(string_strcmp(name ,queue->name),);
        queue = NULL;
    }

    pthread_mutex_unlock(&gsym.qlock);

    return queue;
}

/**
 * @brief 从队列上删除指定节点
 */
static sym_queue_t *__sym_queue_del(const char *name)
{
    struct list_head *pos = NULL ,*tmp = NULL;
    sym_queue_t *queue = NULL;

    pthread_mutex_lock(&gsym.qlock);

    list_for_each_safe(pos ,tmp ,&gsym.queue){
        queue = list_entry(pos ,sym_queue_t ,list);
        assert_continue(!string_strcmp(name ,queue->name),queue = NULL;);

        list_del(pos);
        break;
    }

    pthread_mutex_unlock(&gsym.qlock);

    return queue;
}
/**
 * @brief sym_func_set 
 *   添加一个函数
 */
int sym_func_set(sym_queue_t *queue ,sym_node_t *snode)
{
    list_add(&snode->list ,&queue->qlist);

    pthread_mutex_lock(&gsym.tlock);

    rbtree_push(&gsym.tree ,&snode->rbnode);

    pthread_mutex_unlock(&gsym.tlock);

    return 0;
}

/**
 * @brief sym_func_get 
 *   根据模块名
 */
void *sym_func_get(unsigned char *name)
{
    sym_node_t *snode = NULL;

    pthread_mutex_lock(&gsym.tlock);

    snode = (sym_node_t *)rbtree_search(&gsym.tree ,name); 

    pthread_mutex_unlock(&gsym.tlock);

    return snode ? snode->function : NULL;
}

/**
 * @brief __sym_def_destroy 
 *   函数表清理的时候调用
 */
static void __sym_def_destroy(struct rb_node *node)
{

}

/**
 * @brief __sym_def_compare 
 *   函数查找用到的比较函数
 */
static int __sym_def_compare(unsigned char *k1 ,unsigned char *k2){
    return string_strcmp((const char *)k1 ,(const char *)k2);
}
