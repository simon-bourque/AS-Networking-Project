#pragma once

#include "UDPSocket.h"
#include "TCPSocket.h"

#include <string>

class Client
{
public:
	Client();
	~Client();

private:
	enum class ClientState : uint8 {
		MAIN_MENU,
		DEREGISTERING,
		SENDING_OFFER,
		SENDING_BID,
		DISPLAYING_OFFERS,
		DISPLAYING_BIDS,
		DISPLAYING_WON_ITEMS,
		DISCONNECTING
	};

	// Messages & menu items
	void sendRegister(std::string serverAddress, std::string port);
	void sendDeregister();
	void sendOffer();
	void sendBid();
	void printBids();
	void printOffers();
	void printWonItems();
	void disconnect();

	void printMainMenu();

	void interpretState();

	// Networking functions
	void connect();
	void sendPacket(const Packet& packet);
	void shutdown();

	static int s_reqNum;

	ClientState _state;
	IPV4Address _serverIpv4;
	UDPSocket _udpSocket;
	TCPSocket _tcpSocket;

	std::string _uniqueName;

	bool _registered;
	bool _continue;
};