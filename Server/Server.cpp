#include "Server.h"

#include "Log.h"
#include "UDPSocket.h"

void udpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
void tcpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

Server::Server() : 
	m_listeningUDP(false)
	, m_listeningTCP(false)
{}

Server::~Server() {}

void Server::startUDPServiceThread() {
	ThreadPool::get()->submit(udpServiceExec, this);
}

void Server::startTCPServiceThread() {

}

void udpServiceExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(work);
	CallbackMayRunLong(instance);

	Server* server = reinterpret_cast<Server*>(parameter);

	// Create listen socket
	UDPSocket listenerSocket;

	IPV4Address address("127.0.0.1", DEFAULT_PORT);
	listenerSocket.bind(address);

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

}