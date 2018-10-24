#include "Socket.h"

#include "SocketManager.h"

#include <stdexcept>
#include <ws2tcpip.h>

Socket::Socket(SOCKET winSocket, addrinfo* addressInfo)
	: _winSocket(winSocket)
	, _addressInfo(addressInfo) {}

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

void Socket::listenUDP(uint8* buffer, uint32 bufferSize) {
	// TODO in the future replace with Packet class
	sockaddr senderAddress;

	//int32 numBytesRecieved = recvfrom(_winSocket, buffer, bufferSize, 0, &senderAddress, sizeof(senderAddress));
}

void Socket::sendUDP() {

}

Socket::~Socket() {
	if (_addressInfo != nullptr) {
		freeaddrinfo(_addressInfo);
	}
	closesocket(_winSocket);
}
