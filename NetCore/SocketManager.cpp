#include "SocketManager.h"

#include <stdexcept>

SocketManager* SocketManager::s_instance = nullptr;

SocketManager::SocketManager() {
	// Initialize Winsock
	int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (result != 0) {
		throw std::runtime_error("Help... I've fallen... and I can't get up!");
	}
}


SocketManager::~SocketManager() {
	WSACleanup();
}
