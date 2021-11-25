#ifndef __CLIENT_H__
#define __CLIENT_H__

#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>

#else

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#endif

/* TCP Client */
typedef struct TCP_CLIENT
{
	volatile int run;
	pthread_t thr;
	int sockfd, pos, max;
	char *buf;
	struct sockaddr_in servaddr;
	void (*fnrecv)(const char *);
	void (*fnstatus)(int);
} TCP_Client;

#define TCP_STATUS_DISCONNECTED 1

int tcp_client_init(TCP_Client *cli, int max, void (*status)(int), void (*recv)(const char *));
int tcp_client_connect(TCP_Client *cli, const char *ip, int port);
int tcp_client_send(TCP_Client *cli, const char *msg, int len);
void tcp_client_close(TCP_Client *cli);
void tcp_client_destroy(TCP_Client *cli);

#endif

