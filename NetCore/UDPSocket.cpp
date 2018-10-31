#include "UDPSocket.h"



UDPSocket::UDPSocket(const char* address, const char* port) : Socket(address, port, Socket::SOCKET_TYPE::UDP) {}

void UDPSocket::send(const Packet& packet) {
	int32 numBytesSent = sendto(_winSocket, reinterpret_cast<const char*>(packet.getMessageData()), packet.getMessageSize(), 0, _addressInfo->ai_addr, _addressInfo->ai_addrlen);
}

Packet UDPSocket::receive() {
	// TODO in the future replace with Packet class
	sockaddr senderAddress;
	int32 senderAddressSize = sizeof(senderAddress);

	uint8* buffer = new uint8[Packet::PACKET_SIZE];
	int32 numBytesreceived = recvfrom(_winSocket, reinterpret_cast<char*>(buffer), Packet::PACKET_SIZE, 0, &senderAddress, &senderAddressSize);

	Packet packet(buffer, numBytesreceived);
	delete[] buffer;

	return packet;
}