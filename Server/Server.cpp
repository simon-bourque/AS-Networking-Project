#include "Server.h"

#include "Log.h"
#include "UDPSocket.h"
#include "TCPSocket.h"

#include <iostream>

void udpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

Server::Server(const IPV4Address& bindAddress) : 
	m_listeningUDP(false)
	, m_listeningTCP(false)
	, m_serverBindAddress(bindAddress)
{}

Server::~Server() {}

void Server::startUDPServiceThread() {
	ThreadPool::get()->submit(udpServiceExec, this);
}

void Server::startTCPServiceThread() {
	ThreadPool::get()->submit(tcpServiceExec, this);
}

void udpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);

	// Create listen socket
	UDPSocket listenerSocket;

	listenerSocket.bind(server->m_serverBindAddress);

	log("[INFO] Started listening on UDP port %s", DEFAULT_PORT);

	server->m_listeningUDP = true;
	while (server->m_listeningUDP) {
		Packet packet = listenerSocket.receive();
		MessageType type = static_cast<MessageType>(packet.getMessageData()[0]);
		log(LogType::LOG_RECEIVE, type, packet.getAddress());

		switch (type) {
		case MessageType::MSG_REGISTER:
			RegisterMessage msg = deserializeMessage<RegisterMessage>(packet);

			// REGISTER HIM!
			server->m_connections[packet.getAddress().getSocketAddressAsString()] = Connection();

			RegisteredMessage registeredMsg;
			registeredMsg.reqNum = msg.reqNum;
			memcpy(registeredMsg.name, msg.name, 128);
			memcpy(registeredMsg.iPAddress, msg.iPAddress, 128);
			memcpy(registeredMsg.port, msg.port, 16);

			Packet registeredPacket = serializeMessage(registeredMsg);
			registeredPacket.setAddress(packet.getAddress());

			listenerSocket.send(registeredPacket);
			log(LogType::LOG_SEND, registeredMsg.type, registeredPacket.getAddress());

			break;
		}
	}
}

void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);

	TCPSocket listenerSocket;
	listenerSocket.bind(server->m_serverBindAddress);
	listenerSocket.listen();

	log("[INFO] Started listening on TCP port %s", DEFAULT_PORT);

	server->m_listeningTCP = true;
	while (server->m_listeningTCP) {
		TCPSocket clientSocket = listenerSocket.accept();
		std::cout << "Accepted connection..." << std::endl;

		// Check if connection exists and then set state to connected
		IPV4Address peerAddress = clientSocket.getPeerAddress();
		std::cout << peerAddress.getSocketAddressAsString() << std::endl;
		auto connectionIter = server->m_connections.find(peerAddress.getSocketAddressAsString());
		if (connectionIter != server->m_connections.end()) {

		}
	}
}