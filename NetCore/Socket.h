#pragma once

#include <Winsock2.h>
#include "Types.h"
#include "Packet.h"

#include <string>

static constexpr char DEFAULT_PORT[] = "18081";

class Socket
{
public:
	enum class SOCKET_TYPE
	{
		TCP,
		UDP
	};

	Socket(SOCKET winSocket, addrinfo* addressInfo);
	Socket(const char* address, const char* port, SOCKET_TYPE type);
	Socket(Socket&& sock);
	~Socket();

	void bind();

	virtual void send(const Packet& packet) = 0;
	virtual Packet receive() = 0;

	Socket& operator=(Socket&& sock);
protected:
	std::string WSAErrorCodeToString(int errorCode);

	SOCKET _winSocket;
	addrinfo* _addressInfo;
};

