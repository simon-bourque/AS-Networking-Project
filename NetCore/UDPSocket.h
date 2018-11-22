#pragma once

#include "Socket.h"

class OverlappedBuffer;

class UDPSocket : public Socket {
public:
	UDPSocket(bool overlapped = false);

	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;

	virtual void receiveOverlapped(OverlappedBuffer& overlappedBuffer) override;
};

