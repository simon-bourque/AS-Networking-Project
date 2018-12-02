#pragma once

#include "UDPSocket.h"
#include "TCPSocket.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
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
		DISPLAYING_AH,
		DISCONNECTING
	};

	struct Item {
		std::string description;
		float amount;
	};

	void addUDPAck(uint32 reqNum);
	void removeUDPAck(uint32 reqNum);

	// UDP
	void sendRegister();
	void sendDeregister();
	void sendOffer();
	void sendBid();

	void disconnect();

	void printBids();
	void printOffers();
	void printWonItems();
	void printAH();

	void printMainMenu();

	// TCP
	void updateItemsWon(uint32 itemNum, Item itemWon);
	void updateOffers(uint32 itemNum, Item newOffer);
	void removeOffer(uint32 itemNum);
	void updateBids(uint32 itemNum, Item newBid);
	void removeBid(uint32 itemNum);
	void updateAH(uint32 itemNum, Item newItem);
	void removeAH(uint32 itemNum);

	void interpretState();

	// Networking functions
	void connect();
	void sendPacket(const Packet& packet);
	void shutdown();

	// Thread watching TCP/UDP Socket and updating local tables
	void startTCPWatching();
	void startUDPWatching();
	std::thread _udpWatch;
	std::thread _tcpWatch;

	std::unordered_set<uint32> _tcpAck; // Request numbers to be acknowledged
	std::unordered_set<uint32> _udpAck;

	std::unordered_map<uint32, Item> _wonItems; // Items that this client won
	std::unordered_map<uint32, Item> _offers; // Items the client is currently selling
	std::unordered_map<uint32, Item> _bids; // Items that the client has bid on
	std::unordered_map<uint32, Item> _auctionHouse; // Items available at the auction house
	
	std::mutex _udpAckMtx;
	std::mutex _tcpAckMtx;
	std::mutex _ahMtx;
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