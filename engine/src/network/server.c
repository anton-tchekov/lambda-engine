#include "server.h"
#include "network.h"
#include "types.h"

#ifdef _WIN32

typedef int socklen_t;

#endif

/* TCP Server */
static void *thread_update(void *args);
static int tcp_server_update(TCP_Server *srv);

int tcp_server_init(TCP_Server *srv, int max_clients, int buf_size,
		void (*conn)(int, int), void (*recv)(const char *))
{
	if(!network_enabled())
	{
		return -1;
	}

	if(!(srv->buf = malloc((size_t)(max_clients * buf_size))))
	{
		return -1;
	}

	if(!(srv->sockets = malloc(2UL * (size_t)max_clients * sizeof(*srv->sockets))))
	{
		return -1;
	}

	srv->pos = srv->sockets + max_clients;
	srv->run = 1;
	srv->max = buf_size;
	srv->max_clients = max_clients;
	srv->fnrecv = recv;
	srv->fnconn = conn;
	return 0;
}

int tcp_server_open(TCP_Server *srv, int port)
{
	int opt = 1;
	if(!network_enabled())
	{
		return -1;
	}

	memset(srv->sockets, 0, (size_t)srv->max_clients * sizeof(*srv->sockets));
	memset(srv->pos, 0, (size_t)srv->max_clients * sizeof(*srv->pos));
	if((srv->msocket = (i32)socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		return -1;
	}

	if(setsockopt(srv->msocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)
	{
		return -1;
	}

	srv->address.sin_family = AF_INET;
	srv->address.sin_addr.s_addr = INADDR_ANY;
	srv->address.sin_port = htons((u16)port);
	if(bind(srv->msocket, (struct sockaddr *)&srv->address, sizeof(srv->address)) < 0)
	{
		return -1;
	}

	if(listen(srv->msocket, 3) < 0)
	{
		return -1;
	}

	if(pthread_create(&srv->thr, NULL, thread_update, srv))
	{
		return -1;
	}

	return 0;
}

static void *thread_update(void *args)
{
	TCP_Server *srv = (TCP_Server *)args;
	while(srv->run)
	{
		if(tcp_server_update(srv))
		{
			return NULL;
		}
	}

	return NULL;
}

static int tcp_server_update(TCP_Server *srv)
{
	char *buffer;
	int n, addrlen, new_socket, i, j, k, sd, max_sd;
	fd_set readfds;

	if(!network_enabled())
	{
		return -1;
	}

	addrlen = sizeof(srv->address);
	FD_ZERO(&readfds);
	FD_SET(srv->msocket, &readfds);
	max_sd = srv->msocket;
	for(i = 0; i < srv->max_clients; ++i)
	{
		sd = srv->sockets[i];
		if(sd > 0)
		{
			FD_SET(sd, &readfds);
		}

		if(sd > max_sd)
		{
			max_sd = sd;
		}
	}

	if(select(max_sd + 1, &readfds, NULL, NULL, NULL) < 0 && errno != EINTR)
	{
		return -1;
	}

	if(FD_ISSET(srv->msocket, &readfds))
	{
		if((new_socket = (i32)accept(srv->msocket, (struct sockaddr *)&srv->address, (socklen_t *)&addrlen)) < 0)
		{
			goto skip;
		}

		for(i = 0; i < srv->max_clients; ++i)
		{
			if(srv->sockets[i] == 0)
			{
				srv->sockets[i] = new_socket;
				srv->fnconn(i, 1);
				break;
			}
		}

		if(i == srv->max_clients)
		{
			close(new_socket);
		}
	}

skip:
	for(i = 0; i < srv->max_clients; ++i)
	{
		sd = srv->sockets[i];
		if(!FD_ISSET(sd, &readfds))
		{
			continue;
		}

		buffer = srv->buf + i * srv->max;
		if((n = (int)read(sd, buffer + srv->pos[i], (size_t)(srv->max - srv->pos[i]))) <= 0)
		{
			srv->sockets[i] = 0;
			srv->fnconn(i, 0);
			close(sd);
			continue;
		}

		srv->pos[i] += n;
		j = 0;
		for(k = 0; k < srv->pos[i]; ++k)
		{
			if(buffer[k] == '\n')
			{
				buffer[k] = '\0';
				srv->fnrecv(buffer + j);
				j = k + 1;
			}
		}

		srv->pos[i] -= j;
		memmove(buffer, buffer + srv->pos[i], (size_t)srv->pos[i]);
	}

	return 0;
}

int tcp_server_send(TCP_Server *srv, int client_id, const char *msg, int len)
{
	if(!network_enabled())
	{
		return -1;
	}

	if((int)send(srv->sockets[client_id], msg, (size_t)len, 0) != len)
	{
		return -1;
	}

	return 0;
}

void tcp_server_sendall(TCP_Server *srv, const char *msg, int len)
{
	int i, sd;
	if(!network_enabled())
	{
		return;
	}

	for(i = 0; i < srv->max_clients; ++i)
	{
		sd = srv->sockets[i];
		if(sd == 0)
		{
			continue;
		}

		if((int)send(sd, msg, (size_t)len, 0) != len)
		{
			close(sd);
			srv->sockets[i] = 0;
		}
	}
}

void tcp_server_close(TCP_Server *srv)
{
	int i, sd;
	if(!network_enabled())
	{
		return;
	}

	srv->run = 0;
	pthread_join(srv->thr, NULL);
	for(i = 0; i < srv->max_clients; ++i)
	{
		sd = srv->sockets[i];
		if(sd != 0)
		{
			close(sd);
			srv->sockets[i] = 0;
		}
	}
}

void tcp_server_destroy(TCP_Server *srv)
{
	if(!network_enabled())
	{
		return;
	}

	free(srv->buf);
	free(srv->sockets);
}
