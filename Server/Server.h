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

class Server {
private:
	friend void udpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void tcpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void connectionServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

	std::unordered_map<std::string, Connection> m_connections;

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

	void startBid();
public:
	Server(const IPV4Address& bindAddress);
	virtual ~Server();

	void startUDPServiceThread();
	void startTCPServiceThread();
	void startConnectionServiceThread();

	void shutdown();
};

