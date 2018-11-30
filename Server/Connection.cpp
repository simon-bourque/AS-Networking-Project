#include "Connection.h"

#include "TCPSocket.h"
#include "Log.h"

Connection::Connection() :
	m_state(ConnectionState::CONNECTING)
	, m_tcpSocket(nullptr)
{}

Connection::Connection(const std::string& name, const IPV4Address& address) :
	m_state(ConnectionState::CONNECTING)
	, m_tcpSocket(nullptr)
	, m_uniqueName(name)
	, m_address(address)
{}


Connection::~Connection() {
	delete m_tcpSocket;
}

void Connection::connect(TCPSocket&& socket, HANDLE ioCompletionPort) {
	m_tcpSocket = new TCPSocket(std::move(socket));
	m_state = ConnectionState::CONNECTED;

	CreateIoCompletionPort(m_tcpSocket->getWinSockHandle(), ioCompletionPort, (ULONG_PTR)this, 0);

	m_tcpSocket->receiveOverlapped(m_overlappedBuffer);
}

void Connection::shutdown() {
	m_tcpSocket->shutdown();
	m_tcpSocket->close();
}