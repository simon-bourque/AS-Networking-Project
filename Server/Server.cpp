#include "Server.h"

#include "Log.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "Error.h"

#include <Mswsock.h>
#include <iostream>
#include <mutex>

void udpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
void tcpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
void connectionServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

Server::Server(const IPV4Address& bindAddress) : 
	m_serverBindAddress(bindAddress)
	, m_serverUDPSocket(true)
	, m_serverTCPSocket(true)
	, m_running(true)
{
	m_udpServiceIOPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	m_tcpServiceIOPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	m_connectionServiceIOPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

Server::~Server() {}

void Server::shutdown() {
	m_running = false;
	m_serverUDPSocket.close();
	m_serverTCPSocket.close();
	m_connections.clear();

	PostQueuedCompletionStatus(m_udpServiceIOPort, 0, 0, nullptr);
	PostQueuedCompletionStatus(m_tcpServiceIOPort, 0, 0, nullptr);
	PostQueuedCompletionStatus(m_connectionServiceIOPort, 0, 0, nullptr);
}

void Server::startUDPServiceThread() {
	m_serverUDPSocket.bind(m_serverBindAddress);
	
	CreateIoCompletionPort(m_serverUDPSocket.getWinSockHandle(), m_udpServiceIOPort, 1, 0);

	m_serverUDPSocket.receiveOverlapped(m_serverUDPBuffer);
	ThreadPool::get()->submit(udpServiceRoutine, this);
}

void Server::startTCPServiceThread() {
	m_serverTCPSocket.bind(m_serverBindAddress);
	m_serverTCPSocket.listen();

	CreateIoCompletionPort(m_serverTCPSocket.getWinSockHandle(), m_tcpServiceIOPort, 1, 0);
	
	ThreadPool::get()->submit(tcpServiceRoutine, this);
}

void Server::startConnectionServiceThread() {
	ThreadPool::get()->submit(connectionServiceRoutine, this);
}

void Server::handlePacket(const Packet& packet) {
	MessageType type = static_cast<MessageType>(packet.getMessageData()[0]);
	log(LogType::LOG_RECEIVE, type, packet.getAddress());

	switch (type) {
	case MessageType::MSG_REGISTER:
		handleRegisterPacket(packet);
		break;
	case MessageType::MSG_DEREGISTER:
		handleDeregisterPacket(packet);
		break;
	}

}

void Server::handleRegisterPacket(const Packet& packet) {
	RegisterMessage msg = deserializeMessage<RegisterMessage>(packet);

	// REGISTER HIM!
	m_connections[packet.getAddress().getSocketAddressAsString()] = Connection();

	RegisteredMessage registeredMsg;
	registeredMsg.reqNum = msg.reqNum;
	memcpy(registeredMsg.name, msg.name, NAMELENGTH);
	memcpy(registeredMsg.iPAddress, msg.iPAddress, IPLENGTH);
	memcpy(registeredMsg.port, msg.port, PORTLENGTH);

	Packet registeredPacket = serializeMessage(registeredMsg);
	registeredPacket.setAddress(packet.getAddress());

	m_serverUDPSocket.send(registeredPacket);
	log(LogType::LOG_SEND, registeredMsg.type, registeredPacket.getAddress());
}

void Server::handleDeregisterPacket(const Packet& packet) {
	DeregisterMessage msg = deserializeMessage<DeregisterMessage>(packet);

	// DEREGISTER HIM!
	auto it = m_connections.find(packet.getAddress().getSocketAddressAsString());
	if (it != m_connections.end())
	{
		// User was found in the registered table, remove him
		DeregConfMessage deregConfMsg;
		deregConfMsg.reqNum = msg.reqNum;

		Packet deregConfPacket = serializeMessage(deregConfMsg);
		deregConfPacket.setAddress(packet.getAddress());

		m_serverUDPSocket.send(deregConfPacket);
		log(LogType::LOG_SEND, deregConfMsg.type, deregConfPacket.getAddress());

		m_connections.erase(it);
	}
	else
	{
		// User was not found in the registered table
		DeregDeniedMessage deregDeniedMsg;
		deregDeniedMsg.reqNum = msg.reqNum;

		char reason[REASONLENGTH] = "User was not previously registered";
		memcpy(deregDeniedMsg.reason, reason, REASONLENGTH);

		Packet deregDeniedPacket = serializeMessage(deregDeniedMsg);
		deregDeniedPacket.setAddress(packet.getAddress());

		m_serverUDPSocket.send(deregDeniedPacket);
		log(LogType::LOG_SEND, deregDeniedMsg.type, deregDeniedPacket.getAddress());
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
		bool status = GetQueuedCompletionStatus(server->m_udpServiceIOPort, &numBytes, &key, &overlapped, INFINITE);
		if (!status) {
			// Most likely shutting down
			break;
		}
		if (key == 0) {
			// Shutdown requested
			break;
		}

		// Convert OverlappedBuffer to Packet for ease of use
		OverlappedBuffer& buffer = server->m_serverUDPBuffer;
		Packet packet(buffer.getData(), numBytes);
		packet.setAddress(buffer.getAddress());

		server->handlePacket(packet);

		try {
			server->m_serverUDPSocket.receiveOverlapped(buffer);
		}
		catch (int32 error) {
			log("[ERROR] %s", getWSAErrorString(error).c_str());
			break;
		}
	}
	log("[INFO] UDP service routine shutdown.");
}

void tcpServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);

	DWORD numBytes = 0;
	ULONG_PTR key = 0;
	LPOVERLAPPED overlapped = nullptr;
	TCPSocket acceptedSocket = server->m_serverTCPSocket.acceptOverlapped(server->m_serverTCPBuffer);

	log("[INFO] Started listening on TCP port %s", server->m_serverBindAddress.getSocketPortAsString().c_str());
	while (server->m_running) {
		bool result = GetQueuedCompletionStatus(server->m_tcpServiceIOPort, &numBytes, &key, &overlapped, INFINITE);
		if (!result) {
			// TODO do something when error
			DWORD error = GetLastError();

			if (error == ERROR_ABANDONED_WAIT_0) {
				break;
			}

			std::cout << "[ERROR] " << getWindowsErrorString(error);
			break;
		}
		if (key == 0) {
			// shutdown
			break;
		}
		std::cout << "Accepted connection..." << std::endl;

		SOCKET socketHandle = server->m_serverTCPSocket.getWinSockSocket();
		int32 resultOpt = setsockopt(acceptedSocket.getWinSockSocket(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&socketHandle), sizeof(socketHandle));
		if (resultOpt == SOCKET_ERROR) {
			int32 error = WSAGetLastError();
			std::cout << getWSAErrorString(error) << std::endl;
		}

		IPV4Address peerAddress = acceptedSocket.getPeerAddress();
		std::cout << peerAddress.getSocketAddressAsString() << std::endl;
		auto connectionIter = server->m_connections.find(peerAddress.getSocketAddressAsString());
		if (connectionIter != server->m_connections.end()) {
			connectionIter->second.connect(std::move(acceptedSocket), server->m_connectionServiceIOPort);
		}

		acceptedSocket = server->m_serverTCPSocket.acceptOverlapped(server->m_serverTCPBuffer);
	}

	log("[INFO] TCP service routine shutdown.");
}

void connectionServiceRoutine(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);
	
	DWORD numBytes = 0;
	ULONG_PTR key = 0;
	LPOVERLAPPED overlapped = nullptr;

	while (server->m_running) {
		GetQueuedCompletionStatus(server->m_connectionServiceIOPort, &numBytes, &key, &overlapped, INFINITE);
		if (key == 0) {
			// Shutdown
			break;
		}

		Connection* connection = reinterpret_cast<Connection*>(key);

		OverlappedBuffer& buffer = connection->getOverlappedBuffer();
		Packet packet(buffer.getData(), numBytes);
		
		// Get this from Connection
		//packet.setAddress(buffer.getAddress());

		// Handle packet
		std::cout << "PIIINIGNIGNGINGIGNI" << std::endl;
	}
	log("[INFO] Connection service routine shutdown.");
}