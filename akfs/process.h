#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "akfs.h"

int ProcessMonitor(void);

#endif