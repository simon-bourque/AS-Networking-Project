#pragma once

#include <unordered_map>
#include <string>
#include <thread>

#include "Types.h"

#include "ThreadPool.h"
#include "Connection.h"
#include "IPV4Address.h"
#include "UDPSocket.h"
#include "OverlappedBufferPool.h"

class Server {
private:
	friend void udpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	
	friend void udpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
	friend void workerThreadRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

	friend void udpServiceExec2(PTP_CALLBACK_INSTANCE instance, PVOID context,
		PVOID overlapped, ULONG ioResult, ULONG_PTR numberOfBytesTransferred, PTP_IO io);

	std::unordered_map<std::string, Connection> m_connections;

	bool m_listeningUDP;
	bool m_listeningTCP;

	bool m_running;

	IPV4Address m_serverBindAddress;

	OverlappedBufferHandle m_serverUDPBuffer;
	UDPSocket m_serverUDPSocket;

	HANDLE m_udpServiceIOPort;
public:
	Server(const IPV4Address& bindAddress);
	virtual ~Server();

	void startUDPServiceThread();
	void startTCPServiceThread();

	void shutdown();
};

