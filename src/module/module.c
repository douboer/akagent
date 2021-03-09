#include "module.h"

static struct list_head gm_queue;

/**
 * @brief 初始化模块队列 
 */
static PRE_DEFINE(MODULE_PRI_INIT) void __module_init(void)
{
    list_head_init(&gm_queue);
}

/**
 * @brief module_init 
 *   内置模块初始化
 */
int module_init(config_t *gconfig)
{
    struct list_head *pos = NULL ,*tmp = NULL;
    module_t *m = NULL;
    int ret;

    list_for_each_safe(pos ,tmp ,&gm_queue){
        m = list_entry(pos ,module_t ,list);

        printf("Now run [%s] module\n" ,m->name);

        ret = m->check ? m->check(gconfig) : 0;
        assert_break(!ret ,ret = -EACCES);

        ret = m->init ? m->init(gconfig) : 0;
        assert_break(!ret ,ret = -EACCES);
    }

    return ret;
}

/**
 * @brief module_register 
 *   注册模块接口
 */
void module_register(module_t *m){
    list_add_tail(&m->list ,&gm_queue);
}
