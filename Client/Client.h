#pragma once

#include "UDPSocket.h"
#include "TCPSocket.h"

#include <string>

class Client
{
public:
	Client(std::string uniqueName);
	~Client();

	void sendRegister(std::string serverAddress, std::string port);

private:
	static int s_reqNum;

	UDPSocket _udpSocket;
	TCPSocket _tcpSocket;

	std::string _uniqueName;
	bool _registered;
};

