#pragma once

#include "Socket.h"

class UDPSocket : public Socket {
public:
	UDPSocket(const char* address, const char* port);

	virtual void send(uint8* buffer, uint32 bufferSize) override;
	virtual void receive(uint8* buffer, uint32 bufferSize) override;
};

