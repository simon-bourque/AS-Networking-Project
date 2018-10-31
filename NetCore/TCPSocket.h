#pragma once

#include "Socket.h"

class TCPSocket : public Socket {
public:
	TCPSocket(const char* address, const char* port);
	
	virtual void send(uint8* buffer, uint32 bufferSize) override;
	virtual void receive(uint8* buffer, uint32 bufferSize) override;
};

