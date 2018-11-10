#include "Client.h"

#include "Messages.h"
#include "Log.h"

#include <iostream>

int Client::s_reqNum = 0;

Client::Client(std::string uniqueName)
	: _uniqueName(uniqueName)
	, _registered(false)
{
}

void Client::sendRegister(std::string serverAddress, std::string port)
{
	_serverIpv4 = IPV4Address(serverAddress, DEFAULT_PORT);

	//_udpSocket.setTimeout(5000);

	RegisterMessage registerMsg;
	registerMsg.reqNum = s_reqNum++;
	memcpy(registerMsg.name, _uniqueName.c_str(), _uniqueName.size()+1);
	registerMsg.iPAddress[0] = '\0';
	registerMsg.port[0] = '\0';

	Packet packet = serializeMessage(registerMsg);
	packet.setAddress(_serverIpv4);

	// Attempting and waiting on server for register response
	for (uint32 i = 0; i < 5; i++)
	{
		log(LogType::LOG_SEND, MessageType::MSG_REGISTER, _serverIpv4);

		_udpSocket.send(packet);
		Packet registeredPacket = _udpSocket.receive(); // Blocking call with timeout

		if (registeredPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_REGISTERED))
		{
			// We received a registered packet from the server
			_registered = true;
			break;
		}
	}

	if (_registered)
	{
		log(LogType::LOG_RECEIVE, MessageType::MSG_REGISTERED, _serverIpv4);
	}
	else
	{
		log("ERROR: Could not register to the server");
	}
}

void Client::connect() { _tcpSocket.connect(_serverIpv4); }
void Client::sendPacket(const Packet& packet) { _tcpSocket.send(packet); }
void Client::shutdown()
{ 
	_tcpSocket.shutdown();

	bool receiving = true;
	Packet receivedPacket;
	do {
		receivedPacket = _tcpSocket.receive();
	} while (receivedPacket.getMessageSize() > 0);
}

Client::~Client()
{
}
