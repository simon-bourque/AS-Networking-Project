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
	IPV4Address serverIpv4("127.0.0.1", DEFAULT_PORT);

	//_udpSocket.setTimeout(5000);

	RegisterMessage registerMsg;
	registerMsg.reqNum = s_reqNum++;
	memcpy(registerMsg.name, "SpookySkeleton", 15);
	registerMsg.iPAddress[0] = '\0';
	registerMsg.port[0] = '\0';

	Packet packet = serializeMessage(registerMsg);
	packet.setAddress(serverIpv4);

	// Attempting and waiting on server for register response
	for (uint32 i = 0; i < 5; i++)
	{
		log("Attempting registration to the server...\n");

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
		log(LogType::LOG_RECEIVE, MessageType::MSG_REGISTERED, serverIpv4);
	}
	else
	{
		log("ERROR: Could not register to the server\n");
	}
}

Client::~Client()
{
}
