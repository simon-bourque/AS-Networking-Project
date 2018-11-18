#pragma once

#include <unordered_map>
#include <string>

#include "ThreadPool.h"
#include "Connection.h"
#include "IPV4Address.h"

class Server {
private:
	friend void udpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

	std::unordered_map<std::string, Connection> m_connections;

	bool m_listeningUDP;
	bool m_listeningTCP;

	IPV4Address m_serverBindAddress;
public:
	Server(const IPV4Address& bindAddress);
	virtual ~Server();

	void startUDPServiceThread();
	void startTCPServiceThread();

	void shutdown();
};

