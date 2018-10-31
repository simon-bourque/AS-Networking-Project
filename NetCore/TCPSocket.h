#pragma once

#include "Socket.h"

class TCPSocket : public Socket {
public:
	TCPSocket(const char* address, const char* port);
	
	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;
};

