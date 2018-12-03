#pragma once

#include <unordered_map>
#include <string>
#include <thread>

#include "Types.h"

#include "ThreadPool.h"
#include "Connection.h"
#include "IPV4Address.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "OverlappedBuffer.h"
#include "Item.h"

class Server {
private:
	friend void udpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void tcpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void connectionServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

	std::unordered_map<std::string, Connection> m_connections;
	std::unordered_map<uint32, Item*> m_offeredItems;

	bool m_running;

	IPV4Address m_serverBindAddress;

	OverlappedBuffer m_serverUDPBuffer;
	OverlappedBuffer m_serverTCPBuffer;

	UDPSocket m_serverUDPSocket;
	TCPSocket m_serverTCPSocket;

	HANDLE m_udpServiceIOPort;
	HANDLE m_tcpServiceIOPort;
	HANDLE m_connectionServiceIOPort;

	void handlePacket(const Packet& packet);
	void handleRegisterPacket(const Packet& packet);
	void handleDeregisterPacket(const Packet& packet);
	void handleOfferPacket(const Packet& packet);
	void handleBidPacket(const Packet& packet);

	void sendRegistered(uint32 reqNum, const std::string& name, const std::string& ip, const std::string& port, const IPV4Address& address);
	void sendUnregistered(uint32 reqNum, const std::string& reason, const IPV4Address& address);
	void sendDeregConf(uint32 reqNum, const IPV4Address& address);
	void sendDeregDenied(uint32 reqNum, const std::string& reason, const IPV4Address& address);
	void sendOfferConf(uint32 reqNum, uint32 itemNum, const std::string& description, float32 minimum, const IPV4Address& address);
	void sendOfferDenied(uint32 reqNum, const std::string& reason, const IPV4Address& address);
	void sendNewItem(const Item& item);
	void sendHighest(const Item& item);
	void sendWin(const Item& item);
	void sendBidOver(const Item& item);
	void sendSoldTo(const Item& item);
	void sendNotSold(const Item& item);

public:
	Server(const IPV4Address& bindAddress);
	virtual ~Server();

	void startUDPServiceThread();
	void startTCPServiceThread();
	void startConnectionServiceThread();

	void shutdown();

	void startAuction(const Item& item, uint64 auctionTime = 3000000000ull);
	void bid(uint32 itemID, float32 newBid, const std::string& bidder);
	void endAuction(const Item& item);
	bool isSeller(const std::string& seller);
	bool isHighestBidder(const std::string& bidder);
	int32 getNumOffers(const std::string& seller);

	void saveConnections();
	void loadConnections();
};

