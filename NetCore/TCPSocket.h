#pragma once

#include "Socket.h"

class TCPSocket : public Socket {
public:
	TCPSocket();
	TCPSocket(SOCKET socket);

	// Serverside
	void listen();
	TCPSocket accept();

	// Clientside
	void connect(const IPV4Address& address);
	void shutdown();

	// Hybrid
	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;
};

