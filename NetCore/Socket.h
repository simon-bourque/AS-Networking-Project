#pragma once

#include <winsock2.h>
#include "Types.h"

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

	virtual void send(uint8* buffer, uint32 bufferSize) = 0;
	virtual void receive(uint8* buffer, uint32 bufferSize) = 0;

	Socket& operator=(Socket&& sock);
protected:
	std::string WSAErrorCodeToString(int errorCode);

	SOCKET _winSocket;
	addrinfo* _addressInfo;
};

