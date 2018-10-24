#include "Socket.h"

#include <ws2tcpip.h>

Socket::Socket(SOCKET winSocket, addrinfo* addressInfo)
	: _winSocket(winSocket)
	, _addressInfo(addressInfo) {}

void Socket::bind() {

}

void Socket::listen() {

}

Socket::~Socket() {
	if (_addressInfo != nullptr) {
		freeaddrinfo(_addressInfo);
	}
}
