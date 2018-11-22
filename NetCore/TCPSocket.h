#pragma once

#include "Socket.h"

class OverlappedBuffer;

class TCPSocket : public Socket {
public:
	TCPSocket(bool overlapped = false);
	TCPSocket(SOCKET socket);
	TCPSocket(TCPSocket&& socket);

	TCPSocket& operator=(TCPSocket&& sock);

	// Serverside
	void listen();
	TCPSocket accept();

	TCPSocket acceptOverlapped(OverlappedBuffer& overlappedBuffer);

	// Clientside
	void connect(const IPV4Address& address);
	void shutdown();

	// Hybrid
	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;

	virtual void receiveOverlapped(OverlappedBuffer& overlappedBuffer) override;

	IPV4Address getPeerAddress() const;
};

