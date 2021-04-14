#include "togo.h"

int send2Go(unsigned char *buffer ,unsigned int size,unsigned int usedPort){
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		return -1;
	}

    struct sockaddr_in sockaddrin;
	memset(&sockaddrin, 0, sizeof(sockaddrin));
	sockaddrin.sin_family = AF_INET;
	sockaddrin.sin_port = htons(usedPort);
	sockaddrin.sin_addr.s_addr = inet_addr("127.0.0.1");

    sendto(sock, (char *)buffer, size, 0, (struct sockaddr *)&sockaddrin, sizeof(sockaddrin));
    close(sock);
	return 0;
}