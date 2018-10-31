#include "UDPSocket.h"



UDPSocket::UDPSocket(const char* address, const char* port) : Socket(address, port, Socket::SOCKET_TYPE::UDP) {}

void UDPSocket::send(uint8* buffer, uint32 bufferSize) {
	int32 numBytesSent = sendto(_winSocket, reinterpret_cast<char*>(buffer), bufferSize, 0, _addressInfo->ai_addr, _addressInfo->ai_addrlen);
}

void UDPSocket::receive(uint8* buffer, uint32 bufferSize) {
	// TODO in the future replace with Packet class
	sockaddr senderAddress;
	int32 senderAddressSize = sizeof(senderAddress);

	int32 numBytesreceived = recvfrom(_winSocket, reinterpret_cast<char*>(buffer), bufferSize, 0, &senderAddress, &senderAddressSize);
}