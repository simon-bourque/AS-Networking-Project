#include <iostream>

#include "UDPSocket.h"
#include "TCPSocket.h"
#include "WSA.h"
#include "Messages.h"

int main() {

	WSA::init();

	UDPSocket udpSocket("localhost", DEFAULT_PORT);
	RegisterMessage registerMsg;
	registerMsg.reqNum = 1234;
	memcpy(registerMsg.name, "SpookySkeleton", 15);
	registerMsg.iPAddress[0] = '\0';
	registerMsg.port[0] = '\0';
	uint8* buffer = new uint8[1 + sizeof(registerMsg)];
	buffer[0] = static_cast<uint8>(MessageType::MSG_REGISTER);
	memcpy(buffer + 1, &registerMsg, sizeof(registerMsg));
	udpSocket.send(Packet(buffer, 1 + sizeof(registerMsg)));
	delete[] buffer;
	
	//uint8 testData[3] = { 'a', 'b', 'c' };
	//Packet testPacket(testData, sizeof(testData)/sizeof(testData[0]));
	//
	//
	//TCPSocket tcpSocket("localhost", DEFAULT_PORT);
	//tcpSocket.connect();
	//tcpSocket.send(testPacket);
	//tcpSocket.shutdown();
	//
	//bool receiving = true;
	//Packet receivedPacket;
	//do {
	//	receivedPacket = tcpSocket.receive();
	//} while (receivedPacket.getMessageSize() > 0);

	system("pause");

	WSA::destroy();

	return 0;
}