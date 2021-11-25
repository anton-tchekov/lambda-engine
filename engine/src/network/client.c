#include "client.h"
#include "network.h"
#include "types.h"
#include "thread.h"

#ifdef _WIN32

typedef int socklen_t;

#endif

/* TCP Client */
static void *tcp_thread_update(void *args);
static int tcp_client_update(TCP_Client *cli);

int tcp_client_init(TCP_Client *cli, int max, void (*status)(int), void (*recv)(const char *))
{
	if(!network_enabled())
	{
		return -1;
	}

	if(!(cli->buf = malloc((size_t)max)))
	{
		return -1;
	}

	cli->fnstatus = status;
	cli->fnrecv = recv;
	cli->max = max;
	return 0;
}

int tcp_client_connect(TCP_Client *cli, const char *ip, int port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	char cport[8];
	if(!network_enabled())
	{
		return -1;
	}

	cli->run = 1;
	cli->pos = 0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* TCP */
	hints.ai_flags = 0;
	hints.ai_protocol = 0; /* Any protocol */

	snprintf(cport, sizeof(cport), "%d", port);
	if(getaddrinfo(ip, cport, &hints, &result) != 0)
	{
		return -1;
	}

	for(rp = result; rp != NULL; rp = rp->ai_next)
	{
		cli->sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if(cli->sockfd == -1)
		{
			continue;
		}

		if(connect(cli->sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
		{
			break;
		}

		close(cli->sockfd);
	}

	freeaddrinfo(result);

	if(thread_create(&cli->thr, NULL, tcp_thread_update, cli))
	{
		return -1;
	}

	return 0;
}

static void *tcp_thread_update(void *args)
{
	TCP_Client *cli = (TCP_Client *)args;
	while(cli->run)
	{
		if(tcp_client_update(cli))
		{
			close(cli->sockfd);
			cli->fnstatus(TCP_STATUS_DISCONNECTED);
			return NULL;
		}
	}

	return NULL;
}

static int tcp_client_update(TCP_Client *cli)
{
	int i, j, n;
	if((n = (int)recv(cli->sockfd, cli->buf + cli->pos, (size_t)(cli->max - cli->pos), 0)) <= 0)
	{
		return -1;
	}

	cli->pos += n;
	j = 0;
	for(i = 0; i < cli->pos; ++i)
	{
		if(cli->buf[i] == '\n')
		{
			cli->buf[i] = '\0';
			cli->fnrecv(cli->buf + j);
			j = i + 1;
		}
	}

	cli->pos -= j;
	memmove(cli->buf, cli->buf + cli->pos, (size_t)cli->pos);
	return 0;
}

int tcp_client_send(TCP_Client *cli, const char *msg, int len)
{
	if(!network_enabled())
	{
		return -1;
	}

	if((int)send(cli->sockfd, msg, (size_t)len, 0) != len)
	{
		return -1;
	}

	return 0;
}

void tcp_client_close(TCP_Client *cli)
{
	if(!network_enabled())
	{
		return;
	}

	cli->run = 0;
	pthread_join(cli->thr, NULL);
	close(cli->sockfd);
}

void tcp_client_destroy(TCP_Client *cli)
{
	if(!network_enabled())
	{
		return;
	}

	free(cli->buf);
}
