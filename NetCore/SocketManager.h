#pragma once

#include "Socket.h"

static constexpr char DEFAULT_PORT[] = "18081";

class SocketManager {
private:
	static SocketManager* s_instance;

	WSADATA m_wsaData;
public:
	SocketManager();
	virtual ~SocketManager();

	Socket createTCPSocket() const;
	Socket createUDPSocket() const;

	static void init() { s_instance = new SocketManager(); }
	static void destroy() { delete s_instance; }
	static SocketManager* get() { return s_instance; }
};

