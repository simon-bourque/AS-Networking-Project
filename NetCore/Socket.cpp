#include "Socket.h"

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

}

void Socket::listen() {

}

Socket::~Socket() {
	if (_addressInfo != nullptr) {
		freeaddrinfo(_addressInfo);
	}
	closesocket(_winSocket);
}
