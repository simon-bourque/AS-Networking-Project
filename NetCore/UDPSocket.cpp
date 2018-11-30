#include "UDPSocket.h"

#include <iostream>
#include <Ws2tcpip.h>

#include "OverlappedBuffer.h"
#include "Error.h"

UDPSocket::UDPSocket(bool overlapped) : Socket(Socket::SOCKET_TYPE::UDP, overlapped) {}

void UDPSocket::send(const Packet& packet) {
	const sockaddr* sockAddr = packet.getAddress().getSocketAddress();
	const uint32 sockAddrSize = packet.getAddress().getSocketAddressSize();

	int32 numBytesSent = sendto(_winSocket, reinterpret_cast<const char*>(packet.getMessageData()), packet.getMessageSize(), 0, sockAddr, sockAddrSize);
	if (numBytesSent == SOCKET_ERROR) {
		int32 errorCode = WSAGetLastError();
		throw errorCode;
	}
}

Packet UDPSocket::receive() {
	sockaddr_in senderAddress;
	int32 senderAddressSize = sizeof(senderAddress);

	uint8* buffer = new uint8[Packet::PACKET_SIZE];
	int32 numBytesreceived = recvfrom(_winSocket, reinterpret_cast<char*>(buffer), Packet::PACKET_SIZE, 0, reinterpret_cast<sockaddr*>(&senderAddress), &senderAddressSize);
	if (numBytesreceived == 0) {
		delete[] buffer;
		// Socket shutdown gracefully
		throw 0;
	}
	else if (numBytesreceived == SOCKET_ERROR) {
		delete[] buffer;
		int32 errorCode = WSAGetLastError();
		throw errorCode;
	}

	Packet packet(buffer, numBytesreceived);
	packet.setAddress(IPV4Address(senderAddress));
	delete[] buffer;

	return packet;
}



void UDPSocket::receiveOverlapped(OverlappedBuffer& overlappedBuffer) {
	int32 status = WSARecvFrom(
		_winSocket,
		&overlappedBuffer.m_WSAbuffer,
		1,
		NULL,
		reinterpret_cast<LPDWORD>(&overlappedBuffer.m_flags),
		reinterpret_cast<sockaddr*>(&overlappedBuffer.m_senderAddress),
		&overlappedBuffer.m_senderAddressSize,
		&overlappedBuffer.m_overlapped,
		NULL
	);
	if (status == SOCKET_ERROR) {
		int32 error = WSAGetLastError();
		if (error != WSA_IO_PENDING) {
			throw error;
		}
	}
}