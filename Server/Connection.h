#pragma once

#include "ThreadPool.h"
#include "OverlappedBuffer.h"
#include "IPV4Address.h"

#include <string>
#include <Windows.h>

class TCPSocket;

class Connection {
public:
	enum class ConnectionState {
		DISCONNECTED,
		CONNECTED
	};
private:
	ConnectionState m_state;

	IPV4Address m_address;
	std::string m_uniqueName;

	TCPSocket* m_tcpSocket;
	OverlappedBuffer m_overlappedBuffer;

	uint32 m_offerReqNumber;
public:
	Connection();
	Connection(const std::string& name, const IPV4Address& address);
	virtual ~Connection();

	void connect(TCPSocket&& socket, HANDLE ioCompletionPort);
	void shutdown();

	OverlappedBuffer& getOverlappedBuffer() { return m_overlappedBuffer; }
	const IPV4Address& getAddress() const { return m_address; }

	bool isConnected() const { return m_state == ConnectionState::CONNECTED; }
};

