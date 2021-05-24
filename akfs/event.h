#ifndef __EVENT_H__
#define __EVENT_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "akfs.h"

#define process_used_port  65431
#define file_used_port  65432
#define net_used_port  65433

int ProcessMonitor(void);
int FileMonitor(void);
int NetMonitor(void);

#endif