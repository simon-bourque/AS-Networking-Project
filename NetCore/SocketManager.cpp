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

Socket SocketManager::createUDPSocket() const {
	addrinfo* result = nullptr;
	addrinfo* ptr = nullptr;
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	int status = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (status != 0) {
		throw std::runtime_error("Help... I've fallen... and I can't get up!");
	}

	SOCKET udpSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (udpSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		int errorCode = WSAGetLastError();
		throw std::runtime_error("Failed to create socket: " + SocketManager::WSAErrorCodeToString(errorCode));
	}

	return { udpSocket, result };
}

std::string SocketManager::WSAErrorCodeToString(int errorCode) {
	switch (errorCode) {
	case WSANOTINITIALISED:
		return "WSA not initialized.";
	case WSAENETDOWN:
		return "Network service provider failed.";
	case WSAEAFNOSUPPORT:
		return "Unsupported address family.";
	case WSAEINPROGRESS:
		return "WSA call already in progress.";
	case WSAEMFILE:
		return "No more socket descriptors.";
	case WSAEINVAL:
		return "Invalid argument.";
	case WSAEINVALIDPROVIDER:
		return "Invalid service provider version.";
	case WSAEINVALIDPROCTABLE:
		return "Service provider returned invalid procedure table.";
	case WSAENOBUFS:
		return "No more buffer space.";
	case WSAEPROTONOSUPPORT:
		return "Unsupported protocol.";
	case WSAEPROTOTYPE:
		return "Wrong protocol type for socket.";
	case WSAEPROVIDERFAILEDINIT:
		return "Service provider not initialized.";
	case WSAESOCKTNOSUPPORT:
		return "Unsupported socket type.";
	default:
		return "Unknown error.";
	}
}
