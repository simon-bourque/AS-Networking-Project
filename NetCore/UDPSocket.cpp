#include "UDPSocket.h"

#include <iostream>
#include <Ws2tcpip.h>

UDPSocket::UDPSocket() : Socket(Socket::SOCKET_TYPE::UDP) {}

void UDPSocket::send(const Packet& packet) {
	const sockaddr* sockAddr = packet.getAddress().getSocketAddress();
	const uint32 sockAddrSize = packet.getAddress().getSocketAddressSize();

	int32 numBytesSent = sendto(_winSocket, reinterpret_cast<const char*>(packet.getMessageData()), packet.getMessageSize(), 0, sockAddr, sockAddrSize);
	if (numBytesSent == SOCKET_ERROR) {
		int32 errorCode = WSAGetLastError();
		std::cout << WSAErrorCodeToString(errorCode) << std::endl;
		throw std::runtime_error("Error receiving packet from socket: " + WSAErrorCodeToString(errorCode));
	}
}

Packet UDPSocket::receive() {
	sockaddr_in senderAddress;
	int32 senderAddressSize = sizeof(senderAddress);

	uint8* buffer = new uint8[Packet::PACKET_SIZE];
	int32 numBytesreceived = recvfrom(_winSocket, reinterpret_cast<char*>(buffer), Packet::PACKET_SIZE, 0, reinterpret_cast<sockaddr*>(&senderAddress), &senderAddressSize);
	if (numBytesreceived == SOCKET_ERROR) {
		int32 errorCode = WSAGetLastError();
		std::cout << WSAErrorCodeToString(errorCode) << std::endl;
		throw std::runtime_error("Error receiving packet from socket: " + WSAErrorCodeToString(errorCode));
	}

	Packet packet(buffer, numBytesreceived);
	packet.setAddress(IPV4Address(senderAddress));
	delete[] buffer;

	return packet;
}