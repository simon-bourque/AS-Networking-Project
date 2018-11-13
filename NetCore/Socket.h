#pragma once

#include <Winsock2.h>
#include "Types.h"
#include "Packet.h"
#include "IPV4Address.h"

#include <string>

static constexpr char DEFAULT_PORT[] = "18081";

class Socket {
public:
	enum class SOCKET_TYPE
	{
		TCP,
		UDP
	};

	Socket(SOCKET winSocket);
	Socket(SOCKET_TYPE type, bool blocking = true);
	Socket(Socket&& sock);
	~Socket();

	void bind(const IPV4Address& address);

	virtual void send(const Packet& packet) = 0;
	virtual Packet receive() = 0;
	
	void setTimeout(uint32 ms);

	bool canReceive() const;
	bool canSend() const;

	Socket& operator=(Socket&& sock);
protected:
	std::string WSAErrorCodeToString(int errorCode) const;

	SOCKET _winSocket;
};

