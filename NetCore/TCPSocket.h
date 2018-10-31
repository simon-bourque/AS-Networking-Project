#pragma once

#include "Socket.h"

class TCPSocket : public Socket {
public:
	TCPSocket(const char* address, const char* port);
	TCPSocket(SOCKET socket);

	// Serverside
	void listen();
	TCPSocket accept();

	// Clientside
	void connect();
	void shutdown();

	// Hybrid
	virtual void send(const Packet& packet) override;
	virtual Packet receive() override;
};

