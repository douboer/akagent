/* 
 *屏蔽操作系统差异 
 */

#ifndef __MANGOSTEEN_OS_H__
#define __MANGOSTEEN_OS_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int get_sys_cpus(void);

unsigned int get_sys_tick(void);

#if defined(__USE_UNIX__)

void spawn_init(int argc, char **argv);

void spawn_set_proc(const char *prog, const char *txt);

int use_daemon(void);

#define use_msleep(x) usleep((x) * 1000)

#elif defined(__USE_WIN32__)

#define use_msleep(x) Sleep(x)

#endif //os check

#endif //__MANGOSTEEN_OS_H__
