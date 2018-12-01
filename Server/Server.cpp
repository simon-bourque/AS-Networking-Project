#include "Server.h"

#include "Log.h"
#include "UDPSocket.h"
#include "TCPSocket.h"
#include "Error.h"
#include "Item.h"

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

void Server::startBid() {
	ThreadPool::get()->submitTimer([](PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_TIMER timer) {
		std::cout << "TIMER DONE!" << std::endl;
		CloseThreadpoolTimer(timer);
	}, nullptr);
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
	case MessageType::MSG_OFFER:
		handleOfferPacket(packet);
	}

}

void Server::handleRegisterPacket(const Packet& packet) {
	RegisterMessage msg = deserializeMessage<RegisterMessage>(packet);

	// Attempt to register
	auto iter = m_connections.find(packet.getAddress().getSocketAddressAsString());
	if (iter == m_connections.end()) {
		log("[INFO] Registering client %s (%s)", msg.name, packet.getAddress().getSocketAddressAsString().c_str());
		m_connections[packet.getAddress().getSocketAddressAsString()] = Connection(std::string(msg.name), packet.getAddress());
	}
	else {
		log("[INFO] Client %s (%s) already registered", msg.name, packet.getAddress().getSocketAddressAsString().c_str());
	}

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

		(*it).second.shutdown();
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

void Server::handleOfferPacket(const Packet& packet) {
	OfferMessage msg = deserializeMessage<OfferMessage>(packet);
	
	auto iter = m_connections.find(packet.getAddress().getSocketAddressAsString());
	const bool connected = (iter != m_connections.end()) ? (*iter).second.isConnected() : false;

	if (connected) {
		Connection& connection = (*iter).second;

		// TODO start new bid
		// TODO check if item already offered using reqID
		Item item(std::string(msg.description), msg.minimum);

		// Send confirmation
		OfferConfMessage offerConfMsg;
		offerConfMsg.reqNum = msg.reqNum;
		memcpy(offerConfMsg.description, msg.description, DESCLENGTH);
		offerConfMsg.minimum = msg.minimum;
		offerConfMsg.itemNum = item.getItemID();

		Packet offerConfPacket = serializeMessage(offerConfMsg);
		offerConfPacket.setAddress(packet.getAddress());

		m_serverUDPSocket.send(offerConfPacket);
		log(LogType::LOG_SEND, offerConfMsg.type, offerConfPacket.getAddress());
	}
	else {
		// Client not registered
		OfferDeniedMessage offerDeniedMsg;
		offerDeniedMsg.reqNum = msg.reqNum;
		
		char reason[REASONLENGTH] = "User was not previously registered";
		memcpy(offerDeniedMsg.reason, reason, REASONLENGTH);

		Packet offerDeniedPacket = serializeMessage(offerDeniedMsg);
		offerDeniedPacket.setAddress(packet.getAddress());

		m_serverUDPSocket.send(offerDeniedPacket);
		log(LogType::LOG_SEND, offerDeniedMsg.type, offerDeniedPacket.getAddress());
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
			DWORD error = GetLastError();

			if (error == ERROR_ABANDONED_WAIT_0) {
				break;
			}
			if (error == ERROR_OPERATION_ABORTED) {
				// shutdown
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
		bool result = GetQueuedCompletionStatus(server->m_connectionServiceIOPort, &numBytes, &key, &overlapped, INFINITE);
		Connection* connection = reinterpret_cast<Connection*>(key);
		
		if (!result) {
			DWORD error = GetLastError();

			if (error == ERROR_ABANDONED_WAIT_0) {
				continue;
			}
			if (error == ERROR_OPERATION_ABORTED) {
				// Connection shutdown by server
				continue;
			}
			if (error == ERROR_NETNAME_DELETED) {
				// Ungraceful shutdown (AKA crash on client)
				// TODO delete connection data if not bidding
				connection->shutdown();
				continue;
			}

			std::cout << "[ERROR] " << getWindowsErrorString(error);
			continue;
		}
		if (key == 0) {
			// Server shutdown
			break;
		}
		if (numBytes == 0) {
			// Connection shutdown by client
			// TODO delete connection data if not bidding
			connection->shutdown();
			continue;
		}


		OverlappedBuffer& buffer = connection->getOverlappedBuffer();
		Packet packet(buffer.getData(), numBytes);
		
		// Get this from Connection
		//packet.setAddress(buffer.getAddress());

		// Handle packet
		std::cout << "PIIINIGNIGNGINGIGNI" << std::endl;
	}
	log("[INFO] Connection service routine shutdown.");
}