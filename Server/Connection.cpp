#include "Connection.h"

#include "TCPSocket.h"
#include "Log.h"

void tcpConnectionExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

Connection::Connection() :
	m_state(ConnectionState::CONNECTING)
	, m_tcpSocket(nullptr)
	, m_listening(false)
{}


Connection::~Connection() {
	delete m_tcpSocket;
}

void Connection::connect(TCPSocket&& socket) {
	m_tcpSocket = new TCPSocket(std::move(socket));
	m_state = ConnectionState::CONNECTED;

	ThreadPool::get()->submit(tcpConnectionExec, this);
}

void tcpConnectionExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	Connection* connection = reinterpret_cast<Connection*>(parameter);
	
	connection->m_listening = true;
	while (connection->m_listening) {
		Packet packet = connection->m_tcpSocket->receive();
		log(std::string((const char*)packet.getMessageData(), packet.getMessageSize()).c_str());
	}
}