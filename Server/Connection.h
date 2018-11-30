#pragma once

#include "ThreadPool.h"
#include "OverlappedBuffer.h"

#include <Windows.h>

class TCPSocket;

class Connection {
public:
	enum class ConnectionState {
		CONNECTING,
		CONNECTED
	};
private:
	ConnectionState m_state;

	TCPSocket* m_tcpSocket;
	OverlappedBuffer m_overlappedBuffer;
public:
	Connection();
	virtual ~Connection();

	void connect(TCPSocket&& socket, HANDLE ioCompletionPort);

	OverlappedBuffer& getOverlappedBuffer() { return m_overlappedBuffer; }
};

