#include "lb.h"

static lb_global_t glb;

static int __lb_init(config_t *gconfig);
static void __lb_event_process(void);
static void __lb_event_post_process(struct list_head *queue);

static module_t glb_module = {
    .name = "glb" ,
    .init = __lb_init ,
    .check = NULL,
};

/**
 * @brief PRE_DEFINE 
 *   网络驱动预初始化
 */
static PRE_DEFINE(NDRV_PRI_INIT) void __lb_pre_init(void)
{
#if defined(__USE_UNIX__)
    glb.lb_wait = __lb_epoll_wait;
#elif defined(__USE_WIN32__)
#else
    printf("The system is not supported!\n");
    exit(0);
#endif

    module_register(&glb_module);
}

/**
 * @brief __lb_init 
 *   解析配置文件
 */
static int __lb_init(config_t *gconfig)
{
    list_head_init(&glb.accept_queue);

    list_head_init(&glb.post_queue);

    return 0;
}

/**
* @brief lb_process 
*   请求入口,负载均衡
*/
int lb_process(config_t *gconfig)
{
    //根据配置创建socket
    //
    //添加到网络模型中
   // //
    //等待网络事件
    __lb_event_process();
    return 0;
}

/**
 * @brief __lb_event_process 
 *   lb处理具体请求
 */
static void __lb_event_process(void)
{
    for(;;){
        glb.lb_wait(&glb);

        __lb_event_post_process(&glb.accept_queue);

        __lb_event_post_process(&glb.post_queue);
    }
}

/**
 * @brief __lb_event_post_process 
 *   处理链表上的描述符
 */
static void __lb_event_post_process(struct list_head *queue)
{
    struct list_head *pos = NULL ,*tmp = NULL;
    lb_event_t *lv = NULL;
    int ret;

    list_for_each_safe(pos ,tmp ,queue){
        list_del_init(pos);

        lv = (lb_event_t *)list_entry(pos ,lb_event_t ,list);

        lv->handle(lv);
        //ret = lv->handle(lv);
        //assert_continue(!ret ,);
    }
}
