#include "lb.h"

#ifdef __USE_UNIX__

static int epfd = 0;

static PRE_DEFINE(NDRV_PRI_EPOLL_INIT) void __lb_epoll_init(void)
{
    epfd = epoll_create1(EPOLL_CLOEXEC);
    assert_void(epfd > 0 ,exit(0));
}

int __lb_epoll_add(lb_event_t *lv)
{
    struct epoll_event ev;

    ev.data.ptr = lv;
    ev.events = EPOLLIN | EPOLLPRI;

    return epoll_ctl(epfd ,EPOLL_CTL_ADD ,lv->fd ,&ev);
}

int __lb_epoll_del(lb_event_t *lv)
{
    struct epoll_event ev;

    ev.data.ptr = lv;
    ev.events = EPOLLIN | EPOLLPRI;

    return epoll_ctl(epfd ,EPOLL_CTL_DEL ,lv->fd ,&ev);
}

void __lb_epoll_wait(lb_global_t *glb)
{
    int n ,i;
    lb_event_t *lv = NULL;
    struct list_head *h = NULL;
    struct epoll_event evs[512];

    n = epoll_wait(epfd ,evs ,sizeof(evs) ,500);        
    assert_ret(n >0);

    for(i = 0 ;i < n ;i++){
        lv = (lb_event_t *)evs[i].data.ptr;    

        h = lv->type & LB_EVENT_ACCEPT ? 
            &glb->accept_queue : &glb->post_queue;

        list_add_tail(&lv->list ,h);           
    }
}

#endif  //ifdef __USE_UNIX__
