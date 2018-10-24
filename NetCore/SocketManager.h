#pragma once

#include <Winsock2.h>

class SocketManager {
private:
	static SocketManager* s_instance;

	WSADATA m_wsaData;
public:
	SocketManager();
	virtual ~SocketManager();

	static void init() { s_instance = new SocketManager(); }
	static void destroy() { delete s_instance; }
	static SocketManager* get() { return s_instance; }
};

