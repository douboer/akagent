#include "os.h"

#ifdef __USE_UNIX__
#include <time.h>

/**
* @brief get_sys_tick 
*   获取系统滴答数
*/
unsigned int get_sys_tick(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC ,&ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

#endif
