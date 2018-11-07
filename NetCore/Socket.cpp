#include "Socket.h"

#include <stdexcept>
#include <ws2tcpip.h>

std::string Socket::WSAErrorCodeToString(int errorCode) {
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

Socket::Socket(SOCKET winSocket)
	: _winSocket(winSocket) {}

Socket::Socket(SOCKET_TYPE type) {
	//addrinfo hints;
	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_flags = AI_PASSIVE;
	
	int32 sockType = 0;
	int32 protocol = 0;
	switch (type) {
	case SOCKET_TYPE::TCP:
		sockType = SOCK_STREAM;
		protocol = IPPROTO_TCP;
		break;
	case SOCKET_TYPE::UDP:
		sockType = SOCK_DGRAM;
		protocol = IPPROTO_UDP;
		break;
	}

	// Resolve the local address and port
	//int status = getaddrinfo(address, port, &hints, &_addressInfo);
	//if (status != 0) {
	//	throw std::runtime_error("Help... I've fallen... and I can't get up!");
	//}

	_winSocket = socket(AF_INET, sockType, protocol);
	if (_winSocket == INVALID_SOCKET) {
		//freeaddrinfo(_addressInfo);
		int errorCode = WSAGetLastError();
		throw std::runtime_error("Failed to create socket: " + WSAErrorCodeToString(errorCode));
	}
}

Socket::Socket(Socket&& sock) : _winSocket(sock._winSocket) {
	sock._winSocket = INVALID_SOCKET;
}

Socket& Socket::operator=(Socket&& sock) {
	_winSocket = sock._winSocket;
	sock._winSocket = INVALID_SOCKET;
	return *this;
}

void Socket::bind(const IPV4Address& address) {
	int32 result = ::bind(_winSocket, address.getSocketAddress(), address.getSocketAddressSize());
	if (result == SOCKET_ERROR) {
		int32 errorCode = WSAGetLastError();
		throw std::runtime_error("Failed to create socket: " + WSAErrorCodeToString(errorCode));
	}
}

void Socket::setTimeout(uint32 ms) {
	setsockopt(_winSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(ms), sizeof(uint32));
}

Socket::~Socket() {
	closesocket(_winSocket);
}
