#pragma once

#include "ThreadPool.h"

class TCPSocket;

class Connection {
	friend void tcpConnectionExec(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);
public:
	enum class ConnectionState {
		CONNECTING,
		CONNECTED
	};
private:
	ConnectionState m_state;

	TCPSocket* m_tcpSocket;

	bool m_listening;
public:
	Connection();
	virtual ~Connection();

	void connect(TCPSocket&& socket);
};

