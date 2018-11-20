#pragma once

#include "Socket.h"

class OverlappedBuffer;

class TCPSocket : public Socket {
public:
	TCPSocket();
	TCPSocket(SOCKET socket);
	TCPSocket(TCPSocket&& socket);

	// Serverside
	void listen();
	TCPSocket accept();

	// Clientside
	void connect(const IPV4Address& address);
	void shutdown();

	// Hybrid
	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;

	virtual void receiveOverlapped(OverlappedBufferHandle overlappedBuffer) override;

	IPV4Address getPeerAddress() const;
};

