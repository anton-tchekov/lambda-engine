#include "network.h"
#include "types.h"
#include "log.h"

static int _init;

/* WinSock */
#ifdef _WIN32

#include <winsock2.h>
#include <ws2ipdef.h>

static int _winsock_init(void);
static void _winsock_destroy(void);

#endif

/* Initialize network functions (if necessary) */
int network_init(void)
{
	if(!_init)
	{
		_init = 1;

#ifdef _WIN32
		return _winsock_init();
#endif
	}

	return 0;
}

int network_enabled(void)
{
	return _init;
}

void network_destroy(void)
{
	if(_init)
	{
#ifdef _WIN32
	_winsock_destroy();
#endif
	}
}

/* WinSock */
#ifdef _WIN32

static int _winsock_init(void)
{
	u16 versionRequested;
	WSADATA wsaData;
	versionRequested = MAKEWORD(2, 2);
	if(WSAStartup(versionRequested, &wsaData))
	{
		log_error("WinSock Initialization failed");
		return -1;
	}

	return 0;
}

static void _winsock_destroy(void)
{
	WSACleanup();
}

#endif

