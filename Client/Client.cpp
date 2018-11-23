#include "Client.h"

#include "Messages.h"
#include "Log.h"

#include <iostream>

int Client::s_reqNum = 0;

static char constexpr s_mainMenuString[] = "\n0.Register\n1.Deregister\n2.Send Offer\n3.Send Bid\n4.Display Offers\n5.Display Bids\n6.Display Won Items\n7.Disconnect";

Client::Client()
	: _state(ClientState::MAIN_MENU)
	, _registered(false)
	, _continue(true)
{
	std::cout << "What is your unique name for registration?" << std::endl;
	std::cin >> _uniqueName;

	// TODO REMOVE
	uint8 testData[8] = { 'g','a','r','b','a','g','i','o' };
	Packet testPacket(testData, sizeof(testData) / sizeof(testData[0]));

	// Startup sequence
	while (_continue)
	{
		interpretState();
	}
}

void Client::printMainMenu()
{
	// Print main menu
	std::cout << s_mainMenuString << std::endl;

	// Register user choice
	int chosenOption;
	std::cin >> chosenOption; // Blocking call
	switch (chosenOption)
	{
	case 0:
		_state = ClientState::REGISTER; break;
	case 1:
		_state = ClientState::DEREGISTER; break;
	case 2:
		_state = ClientState::SENDING_OFFER; break;
	case 3:
		_state = ClientState::SENDING_BID; break;
	case 4:
		_state = ClientState::DISPLAYING_OFFERS; break;
	case 5:
		_state = ClientState::DISPLAYING_BIDS; break;
	case 6:
		_state = ClientState::DISPLAYING_WON_ITEMS; break;
	case 7:
		_state = ClientState::DISCONNECTING; break;
	default:
		_state = ClientState::MAIN_MENU;
	}
}

void Client::interpretState()
{
	switch(_state)
	{
	case ClientState::REGISTER:
		sendRegister("127.0.0.1", DEFAULT_PORT); break; // HARD CODED ADDRESS
	case ClientState::DEREGISTER:
		sendDeregister(); break;
	case ClientState::SENDING_OFFER:
		sendOffer(); break;
	case ClientState::SENDING_BID:
		sendBid(); break;
	case ClientState::DISPLAYING_OFFERS:
		printOffers(); break;
	case ClientState::DISPLAYING_BIDS:
		printBids(); break;
	case ClientState::DISPLAYING_WON_ITEMS:
		printWonItems(); break;
	case ClientState::DISCONNECTING:
		disconnect(); break;
	case ClientState::MAIN_MENU:
		printMainMenu(); break;
	default:;
	}
}

void Client::sendRegister(std::string serverAddress, std::string port){
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
		Packet registeredPacket = _udpSocket.receive();

		if (registeredPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_REGISTERED))
		{
			RegisteredMessage registeredMsg = deserializeMessage<RegisteredMessage>(registeredPacket);

			if (registeredMsg.reqNum == registerMsg.reqNum)
			{
				// We received a registered packet from the server
				log(LogType::LOG_RECEIVE, MessageType::MSG_REGISTERED, _serverIpv4);

				_registered = true;

				// We manage to register, try to establish a TCP connection
				connect();

				break;
			}
			else
			{
				// The request numbers do not match? Try again
			}
		}
		else if (registeredPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_UNREGISTERED))
		{
			// An error occurred on the server while registering. Print reason
			log(LogType::LOG_RECEIVE, MessageType::MSG_UNREGISTERED, _serverIpv4);

			UnregisteredMessage unregMsg = deserializeMessage<UnregisteredMessage>(registeredPacket);

			log(unregMsg.reason);
		}
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::sendDeregister()
{
	if (_registered)
	{
		DeregisterMessage deregMsg;
		deregMsg.reqNum = s_reqNum++;
		memcpy(deregMsg.name, _uniqueName.c_str(), _uniqueName.size() + 1);
		deregMsg.iPAddress[0] = '\0';

		Packet packet = serializeMessage(deregMsg);
		packet.setAddress(_serverIpv4);

		// Attempting and waiting on server for register response
		for (uint32 i = 0; i < 5; i++)
		{
			log(LogType::LOG_SEND, MessageType::MSG_DEREGISTER, _serverIpv4);

			_udpSocket.send(packet);
			Packet deregPacket = _udpSocket.receive();

			if (deregPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_DEREG_CONF))
			{
				// We received a confirmation for the deregister everything is gucci
				log(LogType::LOG_RECEIVE, MessageType::MSG_DEREG_CONF, _serverIpv4);
				_registered = false;
				break;
			}
			else if (deregPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_DEREG_DENIED))
			{
				// Oof the deregister was denied. Try again?
				log(LogType::LOG_RECEIVE, MessageType::MSG_DEREG_DENIED, _serverIpv4);
			}
		}
	}
	else
	{
		// Not registered
		log("ERROR: You are not registered");
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::sendOffer()
{
	// CIN the parameters and send the offer to the server
	// Need to check for receive of OFFER_CONF or OFFER_DENIED

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::sendBid()
{
	// CIN the parameters and send the bid to the server

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printBids()
{

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printOffers()
{

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printWonItems()
{

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::disconnect()
{
	_continue = false;
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
	shutdown();
}
