#pragma once

#include "Socket.h"

class UDPSocket : public Socket {
public:
	UDPSocket(const char* address, const char* port);

	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;
};

