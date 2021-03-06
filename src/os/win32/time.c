#include "os.h"

#ifdef __USE_WIN32__

unsigned int get_sys_tick(void){
    return (int)GetTickCount();
}

#endif
