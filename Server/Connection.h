#pragma once

class IPV4Address;

class Connection {
public:
	enum class ConnectionState {
		CONNECTING,
		CONNECTED
	};
private:
	ConnectionState m_state;
public:
	Connection();
	virtual ~Connection();
};

