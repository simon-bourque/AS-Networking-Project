#include "TCPSocket.h"

TCPSocket::TCPSocket(const char* address, const char* port) : Socket(address, port, Socket::SOCKET_TYPE::TCP) {}

void TCPSocket::send(const Packet& packet) {
	//int32 numBytesSent = sendto(_winSocket, reinterpret_cast<char*>(buffer), bufferSize, 0, _addressInfo->ai_addr, _addressInfo->ai_addrlen);
}

Packet TCPSocket::receive() {
	// TODO in the future replace with Packet class

	if (listen(_winSocket, SOMAXCONN) == SOCKET_ERROR) {
		int32 errorCode = WSAGetLastError();
		throw std::runtime_error("Failed to create socket: " + WSAErrorCodeToString(errorCode));
	}

	return Packet();
}