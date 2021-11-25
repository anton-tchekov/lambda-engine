#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#ifdef _WIN32

#include <winsock2.h>
#include <ws2ipdef.h>

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#endif

/* TCP Server */
typedef struct TCP_SERVER
{
	volatile int run;
	pthread_t thr;
	struct sockaddr_in address;
	int max_clients, *sockets, msocket, max, *pos;
	char *buf;
	void (*fnrecv)(const char *);
	void (*fnconn)(int, int);
} TCP_Server;

int tcp_server_init(TCP_Server *srv, int max_clients, int buf_size,
		void (*conn)(int, int), void (*recv)(const char *));
int tcp_server_open(TCP_Server *srv, int port);
int tcp_server_send(TCP_Server *srv, int client_id, const char *msg, int len);
void tcp_server_sendall(TCP_Server *srv, const char *msg, int len);
void tcp_server_close(TCP_Server *srv);
void tcp_server_destroy(TCP_Server *srv);

#endif

