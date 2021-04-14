#ifndef __NET_H__
#define __NET_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "akfs.h"

int NetMonitor(void);

#endif