#ifndef __ANYTHING_CONSTRUCTION_PRI__
#define __ANYTHING_CONSTRUCTION_PRI__

/**
 * @brief 各个模块优先级 
 */
    //模块队列
#define MODULE_PRI_INIT 200 
//扩展模块需要最后在初始化
#define MODULE_PRI_DEF 600

//回调函数模块
#define SYM_PRT_INIT 201

//网络事件模块
#define NDRV_PRI_INIT 300 
#define NDRV_PRI_EPOLL_INIT 301

//定时任务模块
#define TIMING_PRI_INIT 400 


//构造函数定义
#define PRE_DEFINE(x) \
__attribute__((constructor(x)))

#endif
