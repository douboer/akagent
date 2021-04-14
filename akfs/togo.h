#ifndef __TOGO_H__
#define __TOGO_H__

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int send2Go(unsigned char *buffer ,unsigned int size,unsigned int usedPort);

#endif