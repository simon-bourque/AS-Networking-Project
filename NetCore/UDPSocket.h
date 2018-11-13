#pragma once

#include "Socket.h"

class UDPSocket : public Socket {
public:
	UDPSocket(bool blocking = true);

	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;
};

