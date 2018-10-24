#pragma once

#include <Winsock2.h>
#include "Types.h"

class Socket
{
public:
	Socket( SOCKET winSocket, addrinfo* addressInfo);
	Socket(Socket&& sock);
	~Socket();

	void bind();
	void listenUDP(uint8* buffer, uint32 bufferSize);
	void sendUDP();

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

