#pragma once

#include "Socket.h"

class UDPSocket : public Socket {
public:
	UDPSocket();

	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;
};

