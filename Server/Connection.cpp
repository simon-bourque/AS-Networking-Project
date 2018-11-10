#include "Connection.h"

#include "TCPSocket.h"
#include "Log.h"

void tcpConnectionExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

Connection::Connection() :
	m_state(ConnectionState::CONNECTING)
	, m_tcpSocket(nullptr)
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
	
	bool listening = true;
	while (listening) {
		connection->m_tcpSocket->receive();
		log("I GOT SHIT BOOOOI");
	}
}