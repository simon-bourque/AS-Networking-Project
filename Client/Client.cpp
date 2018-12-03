#include "Client.h"

#include "Messages.h"
#include "Log.h"
#include "Strings.h"

#include <iostream>
#include <limits>
#include <chrono>

static constexpr uint32 NUMBEROFTRIES = 5;

int Client::s_reqNum = 1;

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
		_state = ClientState::DISPLAYING_WON_ITEMS; break;
	case 6:
		_state = ClientState::DISPLAYING_AH; break;
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
	case ClientState::DISPLAYING_WON_ITEMS:
		printWonItems(); break;
	case ClientState::DISPLAYING_AH:
		printAH(); break;
	case ClientState::DISCONNECTING:
		disconnect(); break;
	case ClientState::MAIN_MENU:
		printMainMenu(); break;
	default:;
	}
}

void Client::updateAH(uint32 itemNum, Item newItem) {
	std::lock_guard<std::mutex> lock(_ahMtx);

	// Updating auction house table with new amount
	auto it = _auctionHouse.find(itemNum);
	if (it != _auctionHouse.end()) {
		if (it->second.description == "") {
			// We don't have a description, try to update it
			it->second.description = newItem.description;
		}
		it->second.amount = newItem.amount;
	}
	else {
		// This client has never seen this item, add new item to the table
		_auctionHouse[itemNum] = { newItem.description, newItem.amount };
	}
}

void Client::removeAH(uint32 itemNum) {
	std::lock_guard<std::mutex> lock(_ahMtx);

	// Removing previous bid from bid table
	auto it = _auctionHouse.find(itemNum);
	if (it != _auctionHouse.end()) {
		// Previous bid was found, remove it
		_auctionHouse.erase(it);
	}
}

void Client::updateOffers(uint32 itemNum, Item newItem) {
	std::lock_guard<std::mutex> lock(_offersMtx);

	// Removing previous bid from bid table
	auto it = _offers.find(itemNum);
	if (it != _offers.end()) {
		// Previous bid was found, update it
		if (it->second.description == "") {
			// We don't have a description, try to update it
			it->second.description = newItem.description;
		}
		it->second.amount = newItem.amount;
	}
	else {
		// Previous bid was not found, add it
		_offers[itemNum] = { newItem.description, newItem.amount };
	}
}

void Client::removeOffer(uint32 itemNum) {
	std::lock_guard<std::mutex> lock(_offersMtx);

	// Removing previous bid from bid table
	auto it = _offers.find(itemNum);
	if (it != _offers.end()) {
		// Previous bid was found, remove it
		_offers.erase(it);
	}
	else {
		// Previous bid was not found, wtf happened
		log(s_offerNotFound);
	}
}

void Client::updateItemsWon(uint32 itemNum, Item newItem) {
	std::lock_guard<std::mutex> lock(_wonMtx);

	// Removing previous bid from bid table
	auto it = _wonItems.find(itemNum);
	if (it != _wonItems.end()) {
		// Previous bid was found, update it
		if (it->second.description == "") {
			// We don't have a description, try to update it
			it->second.description = newItem.description;
		}
		it->second.amount = newItem.amount;
	}
	else {
		// Previous bid was not found, add it
		_wonItems[itemNum] = { newItem.description, newItem.amount };
	}
}

void Client::addUDPAck(uint32 reqNum) {
	std::lock_guard<std::mutex> lock(_udpAckMtx);
	_udpAck.insert(reqNum);
}

void Client::removeUDPAck(uint32 reqNum) {
	std::lock_guard<std::mutex> lock(_udpAckMtx);
	_udpAck.erase(reqNum);
}

void Client::startUDPWatching() {
	_udpWatch = std::thread([this] {
		while (_continue && _registered) {
			if (_udpSocket.canReceive()) {
				try {
					Packet receivedPacket = _udpSocket.receive(); // Blocking call
					MessageType messageType = static_cast<MessageType>(receivedPacket.getMessageData()[0]);

					switch (messageType)
					{
					case MessageType::MSG_REGISTERED:
					{
						RegisteredMessage registeredMsg = deserializeMessage<RegisteredMessage>(receivedPacket);

						if (_udpAck.find(registeredMsg.reqNum) != _udpAck.end())
						{
							// We received a registered packet from the server
							log(LogType::LOG_RECEIVE, registeredMsg.type, _serverIpv4);

							// We manage to register, try to establish a TCP connection
							_tcpSocket = new TCPSocket();
							connect();

							removeUDPAck(registeredMsg.reqNum);
						}
						else log(s_wrongAck);
						break;
					}
					case MessageType::MSG_UNREGISTERED:
					{
						UnregisteredMessage unregMsg = deserializeMessage<UnregisteredMessage>(receivedPacket);

						if (_udpAck.find(unregMsg.reqNum) != _udpAck.end())
						{
							// An error occurred on the server while registering. Print reason and try again
							log(LogType::LOG_RECEIVE, unregMsg.type, _serverIpv4);

							log(unregMsg.reason);

							removeUDPAck(unregMsg.reqNum);
						}
						else log(s_wrongAck);
						break;
					}
					case MessageType::MSG_DEREG_CONF:
					{
						DeregConfMessage deregConfMsg = deserializeMessage<DeregConfMessage>(receivedPacket);

						if (_udpAck.find(deregConfMsg.reqNum) != _udpAck.end())
						{
							// We received a confirmation for the deregister everything is gucci
							log(LogType::LOG_RECEIVE, deregConfMsg.type, _serverIpv4);
							_registered = false;

							_tcpSocket->shutdown();
							delete _tcpSocket;
							_tcpSocket = nullptr;

							removeUDPAck(deregConfMsg.reqNum);
						}
						else log(s_wrongAck);
						break;
					}
					case MessageType::MSG_DEREG_DENIED:
					{
						DeregDeniedMessage deregDeniedMsg = deserializeMessage<DeregDeniedMessage>(receivedPacket);

						if (_udpAck.find(deregDeniedMsg.reqNum) != _udpAck.end())
						{
							// Oof the deregister was denied. Print reason and try again
							log(LogType::LOG_RECEIVE, deregDeniedMsg.type, _serverIpv4);
							log("[ERROR] %s", deregDeniedMsg.reason);

							removeUDPAck(deregDeniedMsg.reqNum);
						}
						else log(s_wrongAck);
						break;
					}
					case MessageType::MSG_OFFER_CONF:
					{
						OfferConfMessage offerConfMsg = deserializeMessage<OfferConfMessage>(receivedPacket);

						if (_udpAck.find(offerConfMsg.reqNum) != _udpAck.end())
						{
							// We received a confirmation for the offer, good show!
							log(LogType::LOG_RECEIVE, offerConfMsg.type, _serverIpv4);

							// Add item to local table
							updateOffers(offerConfMsg.itemNum, {offerConfMsg.description, offerConfMsg.minimum});
							updateAH(offerConfMsg.itemNum, { offerConfMsg.description, offerConfMsg.minimum });

							removeUDPAck(offerConfMsg.reqNum);
						}
						else log(s_wrongAck);
						break;
					}
					case MessageType::MSG_OFFER_DENIED:
					{
						OfferDeniedMessage offerDeniedMsg = deserializeMessage<OfferDeniedMessage>(receivedPacket);

						if (_udpAck.find(offerDeniedMsg.reqNum) != _udpAck.end())
						{
							// Owie the offer was denied. Print reason and try again
							log(LogType::LOG_RECEIVE, offerDeniedMsg.type, _serverIpv4);

							log(offerDeniedMsg.reason);

							removeUDPAck(offerDeniedMsg.reqNum);
						}
						else log(s_wrongAck);
						break;
					}
					case MessageType::MSG_NEW_ITEM:
					{
						// Note: this message is not an ACK
						NewItemMessage newItemMsg = deserializeMessage<NewItemMessage>(receivedPacket);

						log(LogType::LOG_RECEIVE, newItemMsg.type, _serverIpv4);
						log(s_newItem, newItemMsg.itemNum);

						updateAH(newItemMsg.itemNum, { newItemMsg.description, newItemMsg.minimum });
						break;
					}

					default:;
					}
				}
				catch (int32 error) {
					log(s_serverErrorUDP); // Could not receive UDP packet
				}
			}
		}
	});
}

void Client::startTCPWatching() {
	_tcpWatch = std::thread([this] {
		while (_continue && _registered) {
			if (_tcpSocket == nullptr) break; // TCP Connection was closed, leave thread
			else if (_tcpSocket->canReceive()) {
				try {
					Packet receivedPacket = _tcpSocket->receive(); // Blocking call
					MessageType messageType = static_cast<MessageType>(receivedPacket.getMessageData()[0]);

					switch (messageType)
					{
					case MessageType::MSG_HIGHEST:
					{
						// One of the client's bids was surpassed
						HighestMessage highestMsg = deserializeMessage<HighestMessage>(receivedPacket);

						log(LogType::LOG_RECEIVE, highestMsg.type, _serverIpv4);

						uint32 itemNum = highestMsg.itemNum;
						float32 newAmount = highestMsg.amount;
						std::string description = highestMsg.description;

						log(s_highest);
						log("Item #: %u", itemNum);
						log("New amount: %.2f", newAmount);

						updateAH(itemNum, { description, newAmount });

						if (_offers.find(itemNum) != _offers.end()) {
							updateOffers(itemNum, { description, newAmount });
						}

						break;
					}
					case MessageType::MSG_WIN:
					{
						// This client has won an item!
						WinMessage winMsg = deserializeMessage<WinMessage>(receivedPacket);

						log(LogType::LOG_RECEIVE, winMsg.type, _serverIpv4);

						// Fetching description (read only) from AH because not in win message
						std::string description = "";
						auto it = _auctionHouse.find(winMsg.itemNum);
						if (it != _auctionHouse.end()) {
							description = it->second.description;
						}

						updateItemsWon(winMsg.itemNum, {description, winMsg.amount});

						break;
					}
					case MessageType::MSG_BID_OVER:
					{
						// This bid is over
						BidOverMessage bidOverMsg = deserializeMessage<BidOverMessage>(receivedPacket);

						log(LogType::LOG_RECEIVE, bidOverMsg.type, _serverIpv4);
						log(s_bidOver, bidOverMsg.itemNum, bidOverMsg.amount);

						// Remove item from AH
						removeAH(bidOverMsg.itemNum);

						break;
					}
					case MessageType::MSG_SOLD_TO:
					{
						// One of the client's item was sold
						SoldToMessage soldToMsg = deserializeMessage<SoldToMessage>(receivedPacket);

						log(LogType::LOG_RECEIVE, soldToMsg.type, _serverIpv4);
						log(s_itemSold, soldToMsg.itemNum, soldToMsg.name, soldToMsg.amount);
						
						// Remove item from AH
						removeAH(soldToMsg.itemNum);
						// Remove item from offers
						removeOffer(soldToMsg.itemNum);

						break;
					}
					case MessageType::MSG_NOT_SOLD:
					{
						// One of the client's item did not sell in time
						NotSoldMessage notSoldMsg = deserializeMessage<NotSoldMessage>(receivedPacket);

						log(LogType::LOG_RECEIVE, notSoldMsg.type, _serverIpv4);
						log(s_itemNotSold, notSoldMsg.itemNum, notSoldMsg.reason);

						// Remove item from AH
						removeAH(notSoldMsg.itemNum);
						// Remove item from offers
						removeOffer(notSoldMsg.itemNum);

						break;
					}
					default:;
					}
				}
				catch (int32 error)
				{
					if (error == 0) {
						//TCP Socket was gracefully closed
						log(s_tcpClosed);
					}
					else if (error == WSAECONNRESET) {
						log(s_tcpForceClosed);
						_registered = false;
					}
					else log(s_serverErrorTCP); // Could not receive TCP packet
				}
			}
		}
	});
}

void wait() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void Client::sendRegister() {
	if (!_registered)
	{
		// Making sure the two watch threads are closed
		if (_udpWatch.joinable()) _udpWatch.join();
		if (_tcpWatch.joinable()) _tcpWatch.join();

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
			_registered = true; // Temporary registration

			_udpSocket.send(packet);
			addUDPAck(registerMsg.reqNum);
			log(LogType::LOG_SEND, MessageType::MSG_REGISTER, _serverIpv4);

			// If UDP watch is already running, no need to restart it
			startUDPWatching();

			wait();

			// Checking ACK receipt
			if (_udpAck.find(registerMsg.reqNum) == _udpAck.end()) {
				startTCPWatching();
				break;
			}
			else {
				_registered = false; // Revoke registration as we have not received an acknowledgement
				log(s_notAck);
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

void Client::sendDeregister() {
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
			addUDPAck(deregMsg.reqNum);
			log(LogType::LOG_SEND, MessageType::MSG_DEREGISTER, _serverIpv4);

			wait();

			// Checking ACK receipt
			if (_udpAck.find(deregMsg.reqNum) == _udpAck.end()) {
				// Ending watch threads because no longer registered
				_udpWatch.join();
				_tcpWatch.join();
				break;
			}
			else log(s_notAck);
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

void Client::sendOffer() {
	// CIN the parameters and send the offer to the server
	// Need to check for receive of OFFER_CONF or OFFER_DENIED
	if (_registered)
	{
		std::string description;
		std::cout << "Enter your product description: " << std::endl;
		std::getline(std::cin, description);

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
			addUDPAck(offerMsg.reqNum);
			log(LogType::LOG_SEND, MessageType::MSG_OFFER, _serverIpv4);
			
			wait();
			
			// Checking ACK receipt
			if (_udpAck.find(offerMsg.reqNum) == _udpAck.end()) break;
			else log(s_notAck);
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

void Client::sendBid() {
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

		_tcpSocket->send(packet);

		log(LogType::LOG_SEND, bidMsg.type, _serverIpv4);
	}
	else
	{
		// Not registered
		log(s_notReg);
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printAH() {
	log("The Auction House:");
	log("Description\tItem Number\tAmount");
	// Print all available items at the AH
	for (const auto& item : _auctionHouse) {
		log("%s\t%u\t%.2f", item.second.description.c_str(), item.first, item.second.amount);
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printOffers() {
	// Print items currently being sold by this client
	log("%s's current offers:", _uniqueName.c_str());
	log("Description\tItem Number\tAmount");
	// Print items this client is offering
	for (const auto& offer : _offers) {
		log("%s\t%u\t%.2f", offer.second.description.c_str(), offer.first, offer.second.amount);
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::printWonItems() {
	// Print items that were won by this client
	log("%s's won items:", _uniqueName.c_str());
	log("Description\tItem Number\tAmount");
	// Print items this client has won
	for (const auto& item : _wonItems) {
		log("%s\t%u\t%.2f", item.second.description.c_str(), item.first, item.second.amount);
	}

	// Go back to main menu
	_state = ClientState::MAIN_MENU;
}

void Client::disconnect() {
	_continue = false;
}

void Client::connect() { _tcpSocket->connect(_serverIpv4); }
void Client::sendPacket(const Packet& packet) { _tcpSocket->send(packet); }
void Client::shutdown() {
	if (_tcpSocket != nullptr) {
		_tcpSocket->shutdown();
		delete _tcpSocket;
		_tcpSocket = nullptr;
	}

	if(_udpWatch.joinable()) _udpWatch.join();
	if(_tcpWatch.joinable()) _tcpWatch.join();
}

Client::~Client() {
	shutdown();
}
