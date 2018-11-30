#include "Client.h"

#include "Messages.h"
#include "Log.h"
#include "Strings.h"

#include <iostream>
#include <limits>

static constexpr uint32 NUMBEROFTRIES = 5;

int Client::s_reqNum = 0;

Client::Client(const std::string address, const std::string port)
	: _state(ClientState::MAIN_MENU)
	, _serverIpv4(address, port)
	, _tcpSocket(nullptr)
	, _registered(false)
	, _continue(true)
{
	std::cout << "What is your unique name for registration?" << std::endl;
	std::cin >> _uniqueName;
	std::cin.clear();

	// Startup sequence
	while (_continue)
	{
		interpretState();
	}
}

void Client::printMainMenu()
{
	// Print separator
	std::cout << s_separator << std::endl;

	// Print registered state
	if (_registered)
	{
		std::cout << "Registered as " << _uniqueName << std::endl;
	}
	else
	{
		std::cout << "Not registered" << std::endl;
	}

	// Print main menu
	std::cout << s_mainMenuString << std::endl;

	// Register user choice
	int chosenOption;
	std::cin >> chosenOption;
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cout << std::endl;

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
		sendRegister(); break; // HARD CODED ADDRESS
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

void Client::startWatching() {
	_watchThread = std::thread([this] {

	});
}

void Client::sendRegister(){
	if (!_registered)
	{
		//_udpSocket.setTimeout(5000);

		RegisterMessage registerMsg;
		registerMsg.reqNum = s_reqNum++;
		memcpy(registerMsg.name, _uniqueName.c_str(), _uniqueName.size() + 1);
		registerMsg.iPAddress[0] = '\0';
		registerMsg.port[0] = '\0';

		Packet packet = serializeMessage(registerMsg);
		packet.setAddress(_serverIpv4);

		// Attempting and waiting on server for register response
		for (uint32 i = 0; i < NUMBEROFTRIES; i++)
		{
			_udpSocket.send(packet);
			log(LogType::LOG_SEND, MessageType::MSG_REGISTER, _serverIpv4);

			try
			{
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
						_tcpSocket = new TCPSocket();
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
					UnregisteredMessage unregMsg = deserializeMessage<UnregisteredMessage>(registeredPacket);

					if (unregMsg.reqNum == registerMsg.reqNum)
					{
						// An error occurred on the server while registering. Print reason and try again
						log(LogType::LOG_RECEIVE, MessageType::MSG_UNREGISTERED, _serverIpv4);

						log(unregMsg.reason);
					}
					else
					{
						// Request numbers do not match
					}
				}
				else
				{
					// Uuhh this was not meant for us
				}
			}
			catch (int32 error)
			{
				log(s_serverClosed); // Could not receive UDP packet
			}
		}
	}
	else
	{
		// User already registered
		log(s_alreadyReg);
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
		for (uint32 i = 0; i < NUMBEROFTRIES; i++)
		{
			_udpSocket.send(packet);
			log(LogType::LOG_SEND, MessageType::MSG_DEREGISTER, _serverIpv4);

			try {
				Packet deregPacket = _udpSocket.receive();

				if (deregPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_DEREG_CONF))
				{
					DeregConfMessage deregConfMsg = deserializeMessage<DeregConfMessage>(deregPacket);

					if (deregConfMsg.reqNum == deregMsg.reqNum)
					{
						// We received a confirmation for the deregister everything is gucci
						log(LogType::LOG_RECEIVE, MessageType::MSG_DEREG_CONF, _serverIpv4);
						_registered = false;

						_tcpSocket->shutdown();
						delete _tcpSocket;
						_tcpSocket = nullptr;

						break;
					}
					else
					{
						// The request numbers do not match? Try again
					}
				}
				else if (deregPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_DEREG_DENIED))
				{
					DeregDeniedMessage deregDeniedMsg = deserializeMessage<DeregDeniedMessage>(deregPacket);

					if (deregDeniedMsg.reqNum == deregMsg.reqNum)
					{
						// Oof the deregister was denied. Print reason and try again
						log(LogType::LOG_RECEIVE, MessageType::MSG_DEREG_DENIED, _serverIpv4);

						log(deregDeniedMsg.reason);
					}
					else
					{
						// Request numbers do not match
					}
				}
				else
				{
					// Uuhh this was not meant for us
				}
			}
			catch (int32 error)
			{
				log(s_serverClosed); // Could not receive UDP packet
			}
		}
	}
	else
	{
		// Not registered
		log(s_notReg);
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::sendOffer()
{
	// CIN the parameters and send the offer to the server
	// Need to check for receive of OFFER_CONF or OFFER_DENIED
	if (_registered)
	{
		std::string description;
		std::cout << "Enter your product description: " << std::endl;
		std::cin >> description;

		float minimum;
		std::cout << "At what price should the auction start?: " << std::endl;
		std::cin >> minimum;

		OfferMessage offerMsg;
		offerMsg.reqNum = s_reqNum++;
		memcpy(offerMsg.name, _uniqueName.c_str(), _uniqueName.size() + 1);
		offerMsg.iPAddress[0] = '\0';
		memcpy(offerMsg.description, description.c_str(), description.size() + 1);
		offerMsg.minimum = minimum;

		Packet packet = serializeMessage(offerMsg);
		packet.setAddress(_serverIpv4);

		// Attempting and waiting on server for offer response
		for (uint32 i = 0; i < NUMBEROFTRIES; i++)
		{
			_udpSocket.send(packet);
			log(LogType::LOG_SEND, MessageType::MSG_OFFER, _serverIpv4);

			try
			{
				Packet offerPacket = _udpSocket.receive();

				if (offerPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_OFFER_CONF))
				{
					OfferConfMessage offerConfMsg = deserializeMessage<OfferConfMessage>(offerPacket);

					if (offerConfMsg.reqNum == offerMsg.reqNum)
					{
						// We received a confirmation for the offer, good show!
						log(LogType::LOG_RECEIVE, MessageType::MSG_OFFER_CONF, _serverIpv4);

						// Add item to local table

						break;
					}
					else
					{
						// Request numbers do not match 
					}
				}
				else if (offerPacket.getMessageData()[0] == static_cast<uint8>(MessageType::MSG_OFFER_DENIED))
				{
					OfferDeniedMessage offerDeniedMsg = deserializeMessage<OfferDeniedMessage>(offerPacket);

					if (offerDeniedMsg.reqNum == offerMsg.reqNum)
					{
						// Owie the offer was denied. Print reason and try again
						log(LogType::LOG_RECEIVE, MessageType::MSG_DEREG_DENIED, _serverIpv4);

						log(offerDeniedMsg.reason);
					}
					else
					{
						// Request numbers do not match
					}
				}
				else
				{
					// Uuhh this was not meant for us
				}
			}
			catch (int32 error)
			{
				log(s_serverClosed); // Could not receive UDP packet
			}
		}
	}
	else
	{
		// Not registered
		log(s_notReg);
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::sendBid()
{
	// CIN the parameters and send the bid to the 
	if (_registered)
	{
		uint32 itemNum;
		std::cout << "Enter the item number: " << std::endl;
		std::cin >> itemNum;

		float amount;
		std::cout << "What amount do you want to bid?: " << std::endl;
		std::cin >> amount;

		BidMessage bidMsg;
		bidMsg.reqNum = s_reqNum++;
		bidMsg.itemNum = itemNum;
		bidMsg.amount = amount;

		Packet packet = serializeMessage(bidMsg);
		packet.setAddress(_serverIpv4);

		// Attempting and waiting on server for offer response
		for (uint32 i = 0; i < NUMBEROFTRIES; i++)
		{
			_udpSocket.send(packet);
			log(LogType::LOG_SEND, MessageType::MSG_BID, _serverIpv4);

			// Packet offerPacket = _udpSocket.receive(); 

			// Check for new highest?
		}

	}
	else
	{
		// Not registered
		log(s_notReg);
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printBids()
{
	// Print bids this client has put on items other than their own

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printOffers()
{
	// Print items currently being sold by this client

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printWonItems()
{
	// Print items that were won by this client

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::disconnect()
{
	_continue = false;
}

void Client::connect() { _tcpSocket->connect(_serverIpv4); }
void Client::sendPacket(const Packet& packet) { _tcpSocket->send(packet); }
void Client::shutdown()
{
	_tcpSocket->shutdown();

	bool receiving = true;
	Packet receivedPacket;
	while(_tcpSocket->canReceive()) {
		receivedPacket = _tcpSocket->receive();
	}

	delete _tcpSocket;
	_tcpSocket = nullptr;
}

Client::~Client()
{
	shutdown();
}
