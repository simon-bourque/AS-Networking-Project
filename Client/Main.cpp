#include <iostream>

#include "UDPSocket.h"
#include "TCPSocket.h"
#include "WSA.h"
#include "Messages.h"

int main() {

	WSA::init();

	UDPSocket udpSocket("localhost", DEFAULT_PORT);
	udpSocket.setTimeout(5000);

	RegisterMessage registerMsg;
	registerMsg.reqNum = 1234;
	memcpy(registerMsg.name, "SpookySkeleton", 15);
	registerMsg.iPAddress[0] = '\0';
	registerMsg.port[0] = '\0';

	Packet packet = serializeMessage(registerMsg);
	udpSocket.send(packet);

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