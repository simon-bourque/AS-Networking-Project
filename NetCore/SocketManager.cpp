#include "SocketManager.h"

#include <stdexcept>
#include <ws2tcpip.h>

SocketManager* SocketManager::s_instance = nullptr;

SocketManager::SocketManager() {
	// Initialize Winsock
	int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (result != 0) {
		throw std::runtime_error("Could not initialize Winsock.");
	}
}

Socket SocketManager::createTCPSocket() const
{
	addrinfo* result = NULL;
	addrinfo* ptr = NULL;
	addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve server address and port
	int iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		throw std::runtime_error("getaddrinfo failed!");
	}

	SOCKET connectSock = INVALID_SOCKET;

	// Attempt to connect to the first address returned by the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	connectSock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	// Check if socket is valid
	if (connectSock == INVALID_SOCKET) {
		freeaddrinfo(result);
		throw std::runtime_error("Invalid socket!");
	}

	return {connectSock, ptr};
}


SocketManager::~SocketManager() {
	WSACleanup();
}