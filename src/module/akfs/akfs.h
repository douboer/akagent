#ifndef __ALWAYSKNOW_FS_H__
#define __ALWAYSKNOW_FS_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ring.h"

typedef struct akfs_process_s{
    unsigned int type;
    pid_t pid;
    pid_t ppid;
    uid_t uid;
    unsigned int ns;
    char tpath[256];
    char ppath[256];
}akfs_process_t;

#define PAGE_SIZE 4096

#endif
