#pragma once

#include "Socket.h"

class OverlappedBuffer;

class TCPSocket : public Socket {
public:
	TCPSocket(bool overlapped = false);
	TCPSocket(SOCKET socket);
	TCPSocket(TCPSocket&& socket);

	// Serverside
	void listen();
	TCPSocket accept();

	TCPSocket acceptOverlapped(OverlappedBufferHandle overlappedBufferHandle);

	// Clientside
	void connect(const IPV4Address& address);
	void shutdown();

	// Hybrid
	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;

	virtual void receiveOverlapped(OverlappedBufferHandle overlappedBufferHandle) override;

	IPV4Address getPeerAddress() const;
};

