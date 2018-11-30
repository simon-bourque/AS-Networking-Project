#pragma once

#include "UDPSocket.h"
#include "TCPSocket.h"

#include <string>
#include <vector>
#include <thread>
#include <mutex>

class Client
{
public:
	Client(const std::string address, const std::string port);
	~Client();

private:
	enum class ClientState : uint8 {
		MAIN_MENU,
		REGISTER,
		DEREGISTER,
		SENDING_OFFER,
		SENDING_BID,
		DISPLAYING_OFFERS,
		DISPLAYING_BIDS,
		DISPLAYING_WON_ITEMS,
		DISCONNECTING
	};

	struct Item {
		std::string description;
		float amount;
		uint32 itemNum;
	};

	// UDP
	void sendRegister();
	void sendDeregister();
	void sendOffer();
	void sendBid();

	void disconnect();

	void printBids(); // TODO
	void printOffers(); // TODO
	void printWonItems(); // TODO

	void printMainMenu();

	// TCP
	void updateItemsWon() {}; // TODO
	void updateOffers() {}; // TODO
	void updateBids() {}; // TODO

	void interpretState();

	// Networking functions
	void connect();
	void sendPacket(const Packet& packet);
	void shutdown();

	// Thread watching TCP Socket and updating local tables
	void startWatching(); // TODO
	std::vector<Item> _wonItems;
	std::vector<Item> _offers;
	std::vector<Item> _bids;
	std::thread _watchThread;
	std::mutex _wonMtx;
	std::mutex _offersMtx;
	std::mutex _bidsMtx;

	static int s_reqNum;

	ClientState _state;

	IPV4Address _serverIpv4;
	UDPSocket _udpSocket;
	TCPSocket* _tcpSocket;

	std::string _uniqueName;

	bool _registered;
	bool _continue;
};