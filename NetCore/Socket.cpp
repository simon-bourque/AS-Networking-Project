#include "Socket.h"

#include "SocketManager.h"

#include <stdexcept>
#include <ws2tcpip.h>

Socket::Socket(SOCKET winSocket, addrinfo* addressInfo)
	: _winSocket(winSocket)
	, _addressInfo(addressInfo) {}

Socket::Socket(const char* address, const char* port, SOCKET_TYPE type) {
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	
	switch (type) {
	case SOCKET_TYPE::TCP:
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		break;
	case SOCKET_TYPE::UDP:
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		break;
	}

	// Resolve the local address and port
	int status = getaddrinfo(address, port, &hints, &_addressInfo);
	if (status != 0) {
		throw std::runtime_error("Help... I've fallen... and I can't get up!");
	}

	_winSocket = socket(_addressInfo->ai_family, _addressInfo->ai_socktype, _addressInfo->ai_protocol);
	if (_winSocket == INVALID_SOCKET) {
		freeaddrinfo(_addressInfo);
		int errorCode = WSAGetLastError();
		throw std::runtime_error("Failed to create socket: " + SocketManager::WSAErrorCodeToString(errorCode));
	}
}

Socket::Socket(Socket&& sock) : _winSocket(sock._winSocket), _addressInfo(_addressInfo) {
	sock._winSocket = INVALID_SOCKET;
	sock._addressInfo = nullptr;
}

Socket& Socket::operator=(Socket&& sock) {
	_winSocket = sock._winSocket;
	_addressInfo = sock._addressInfo;
	sock._winSocket = INVALID_SOCKET;
	sock._addressInfo = nullptr;
	return *this;
}

void Socket::bind() {
	int32 result = ::bind(_winSocket, _addressInfo->ai_addr, static_cast<int32>(_addressInfo->ai_addrlen));
	if (result == SOCKET_ERROR) {
		int32 errorCode = WSAGetLastError();
		throw std::runtime_error("Failed to create socket: " + SocketManager::WSAErrorCodeToString(errorCode));
	}
}

Socket::~Socket() {
	if (_addressInfo != nullptr) {
		freeaddrinfo(_addressInfo);
	}
	closesocket(_winSocket);
}
