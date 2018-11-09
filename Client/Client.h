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
	void connect();
	void sendPacket(const Packet& packet);
	void shutdown();
private:
	static int s_reqNum;

	IPV4Address _serverIpv4;
	UDPSocket _udpSocket;
	TCPSocket _tcpSocket;

	std::string _uniqueName;

	bool _registered;
};

