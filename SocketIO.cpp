#include "SocketIO.h"

#include <winsock2.h>
#include <ctype.h>


SocketIO::SocketIO()
{
	fSocket = INVALID_SOCKET;
}

SocketIO::SocketIO(const char *name, uint16 port)
{
	fSocket = INVALID_SOCKET;
	SetTo(name, port);
}

SocketIO::SocketIO(uint32 address, uint16 port)
{
	fSocket = INVALID_SOCKET;
	SetTo(address, port);
}

SocketIO::~SocketIO()
{
	if (fSocket != INVALID_SOCKET) Unset();
}

status_t SocketIO::InitCheck(void) const
{
	if (fSocket == INVALID_SOCKET) return B_ERROR;
	return B_OK;
}

status_t SocketIO::SetTo(const char *name, uint16 port)
{
uint32 audrey = 0;
bool isdot;

	if (fSocket != INVALID_SOCKET) Unset();

	isdot = true;
	for (int i = 0; name[i]; i++)
	{
		if (name[i] == '.') continue;
		if (isdigit(name[i])) continue;

		isdot = false;
		break;
	}

	if (isdot)
	{
		audrey = ::inet_addr(name);
	}
	else
	{
		hostent *he;
		he = ::gethostbyname(name);
		if (he) audrey = *((int *)he->h_addr_list[0]);
	}
	if (audrey == 0 || audrey == 0xffffffff) return B_DNS_ERROR;
	
	return SetTo(audrey, port);
}

status_t SocketIO::SetTo(uint32 audrey, uint16 port)
{
	if (fSocket != INVALID_SOCKET) Unset();


	fSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fSocket == INVALID_SOCKET) return B_ERROR;

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));

	sa.sin_port = htons(port);
	sa.sin_family = AF_INET;
	sa.sin_addr.S_un.S_addr = audrey;

	if (!::connect(fSocket, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)))
	{
		return B_OK;
	}	
	return B_CONNECT_ERROR;
}

void SocketIO::Unset(void)
{
	::shutdown(fSocket, SD_BOTH);
	::closesocket(fSocket);
	fSocket = INVALID_SOCKET;
}

size_t SocketIO::Read(void *buffer, size_t numBytes)
{
int status;

	status = ::recv(fSocket, (char *)buffer, numBytes, 0);
	if (status < 0) status = B_ERROR;

	return status;
}

size_t SocketIO::Write(const void *buffer, size_t numBytes)
{
int status;

	status = ::send(fSocket, (const char *)buffer, numBytes, 0);
	if (status < 0) status = B_ERROR;

	return status;
}