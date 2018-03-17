#ifndef __SOCKETIO_H__
#define __SOCKETIO_H__

#include "DataIO.h"

#define B_DNS_ERROR -1
#define B_CONNECT_ERROR -2

class SocketIO: public BDataIO
{
public:
	SocketIO();
	SocketIO(const char *name, uint16 port);
	SocketIO(uint32 address, uint16 port);
virtual ~SocketIO();


status_t SetTo(const char *name, uint16 port);
status_t SetTo(uint32 address, uint16 port);
status_t InitCheck(void) const;

void	Unset();

virtual size_t Read(void *buffer, size_t numBytes);
virtual size_t Write(const void *buffer, size_t numBytes);

private:

uint32 fSocket;

};

#endif