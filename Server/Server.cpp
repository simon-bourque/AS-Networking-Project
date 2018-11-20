#include "Server.h"

#include "Log.h"
#include "UDPSocket.h"
#include "TCPSocket.h"

#include <iostream>
#include <mutex>

std::once_flag flag;

void udpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
void tcpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

void workerThreadRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

Server::Server(const IPV4Address& bindAddress) : 
	m_listeningUDP(false)
	, m_listeningTCP(false)
	, m_serverBindAddress(bindAddress)
	, m_serverUDPSocket(true)
	, m_serverTCPSocket(true)
	, m_running(true)
{
	m_udpServiceIOPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	m_tcpServiceIOPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

Server::~Server() {}

void Server::shutdown() {
	// TODO how to close io completion port plz?
	std::call_once(flag, [this]() {
		m_listeningUDP = false;
		m_listeningTCP = false;
	});
}

void Server::startUDPServiceThread() {
	m_serverUDPBufferHandle = OverlappedBufferPool::get()->requestOverlappedBuffer(nullptr);
	m_serverUDPSocket.bind(m_serverBindAddress);
	
	CreateIoCompletionPort(m_serverUDPSocket.getWinSockHandle(), m_udpServiceIOPort, 1, 0);

	m_serverUDPSocket.receiveOverlapped(m_serverUDPBufferHandle);
	ThreadPool::get()->submit(udpServiceRoutine, this);
}

void Server::startTCPServiceThread() {
	//m_serverTCPBufferHandle = OverlappedBufferPool::get()->requestOverlappedBuffer(nullptr);
	//m_serverTCPSocket.bind(m_serverBindAddress);
	//m_serverTCPSocket.listen();
	//
	//CreateIoCompletionPort(m_serverTCPSocket.getWinSockHandle(), m_tcpServiceIOPort, 1, 0);
	
	//ThreadPool::get()->submit(tcpServiceRoutine, this);
	
	ThreadPool::get()->submit(tcpServiceExec, this);
}

void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);

	TCPSocket listenerSocket;
	listenerSocket.bind(server->m_serverBindAddress);
	listenerSocket.listen();

	log("[INFO] Started listening on TCP port %s", server->m_serverBindAddress.getSocketPortAsString().c_str());

	server->m_listeningTCP = true;
	while (server->m_listeningTCP) {
		TCPSocket clientSocket = listenerSocket.accept();
		std::cout << "Accepted connection..." << std::endl;

		// Check if connection exists and then set state to connected
		IPV4Address peerAddress = clientSocket.getPeerAddress();
		std::cout << peerAddress.getSocketAddressAsString() << std::endl;
		auto connectionIter = server->m_connections.find(peerAddress.getSocketAddressAsString());
		if (connectionIter != server->m_connections.end()) {
			connectionIter->second.connect(std::move(clientSocket));
		}
	}
}

void udpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);

	DWORD numBytes = 0;
	ULONG_PTR key = 0;
	LPOVERLAPPED overlapped = nullptr;

	log("[INFO] Started listening on UDP port %s", server->m_serverBindAddress.getSocketPortAsString().c_str());
	while (server->m_running) {
		GetQueuedCompletionStatus(server->m_udpServiceIOPort, &numBytes, &key, &overlapped, INFINITE);
		
		// Convert OverlappedBuffer to Packet for ease of use
		OverlappedBuffer& buffer = OverlappedBufferPool::get()->getOverlappedBuffer(server->m_serverUDPBufferHandle);
		Packet packet(buffer.getData(), numBytes);
		packet.setAddress(buffer.getAddress());

		MessageType type = static_cast<MessageType>(packet.getMessageData()[0]);
		log(LogType::LOG_RECEIVE, type, packet.getAddress());

		switch (type) {
		case MessageType::MSG_REGISTER:
			RegisterMessage msg = deserializeMessage<RegisterMessage>(packet);

			// REGISTER HIM!
			server->m_connections[packet.getAddress().getSocketAddressAsString()] = Connection();

			RegisteredMessage registeredMsg;
			registeredMsg.reqNum = msg.reqNum;
			memcpy(registeredMsg.name, msg.name, NAMELENGTH);
			memcpy(registeredMsg.iPAddress, msg.iPAddress, IPLENGTH);
			memcpy(registeredMsg.port, msg.port, PORTLENGTH);

			Packet registeredPacket = serializeMessage(registeredMsg);
			registeredPacket.setAddress(packet.getAddress());

			server->m_serverUDPSocket.send(registeredPacket);
			log(LogType::LOG_SEND, registeredMsg.type, registeredPacket.getAddress());

			break;
		}

		server->m_serverUDPSocket.receiveOverlapped(server->m_serverUDPBufferHandle);
	}
}

void tcpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);

	DWORD numBytes = 0;
	ULONG_PTR key = 0;
	LPOVERLAPPED overlapped = nullptr;

	log("[INFO] Started listening on TCP port %s", server->m_serverBindAddress.getSocketPortAsString().c_str());
	while (server->m_running) {
		GetQueuedCompletionStatus(server->m_tcpServiceIOPort, &numBytes, &key, &overlapped, INFINITE);

		std::cout << "PING" << std::endl;
	}
}

void workerThreadRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);
	
	DWORD numBytes = 0;
	ULONG_PTR key = 0;
	LPOVERLAPPED overlapped = nullptr;

	while (server->m_running) {
		GetQueuedCompletionStatus(server->m_udpServiceIOPort, &numBytes, &key, &overlapped, INFINITE);

		// Find connection from key
	}
}