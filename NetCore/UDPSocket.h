#pragma once

#include "Socket.h"
#include <string>

class UDPSocket : public Socket {
public:
	UDPSocket(const char* address, const char* port);

	virtual void send(uint8* buffer, uint32 bufferSize) override;
	virtual void recieve(uint8* buffer, uint32 bufferSize) override;
};

