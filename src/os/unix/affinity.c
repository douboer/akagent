#include "os.h"

#ifdef __USE_UNIX__

int get_sys_cpus(void){
    return sysconf(_SC_NPROCESSORS_ONLN);
}

#endif
