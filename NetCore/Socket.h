#pragma once

#include <Winsock2.h>

class Socket
{
public:
	Socket( SOCKET winSocket, addrinfo* addressInfo);
	Socket(Socket&& sock);
	~Socket();

	void bind();
	void listen();

	Socket& operator=(Socket&& sock);
private:
	enum class SOCKET_TYPE
	{
		TCP,
		UDP
	} _type;

	SOCKET _winSocket;
	addrinfo* _addressInfo;
};

