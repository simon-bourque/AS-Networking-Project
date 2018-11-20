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
#include "OverlappedBufferPool.h"

class Server {
private:
	friend void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	
	friend void udpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void tcpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void workerThreadRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

	std::unordered_map<std::string, Connection> m_connections;

	bool m_listeningUDP;
	bool m_listeningTCP;

	bool m_running;

	IPV4Address m_serverBindAddress;

	OverlappedBufferHandle m_serverUDPBufferHandle;
	OverlappedBufferHandle m_serverTCPBufferHandle;

	UDPSocket m_serverUDPSocket;
	TCPSocket m_serverTCPSocket;

	HANDLE m_udpServiceIOPort;
	HANDLE m_tcpServiceIOPort;
public:
	Server(const IPV4Address& bindAddress);
	virtual ~Server();

	void startUDPServiceThread();
	void startTCPServiceThread();

	void shutdown();
};

