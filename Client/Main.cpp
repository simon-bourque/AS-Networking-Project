#include <iostream>

#include "UDPSocket.h"
#include "TCPSocket.h"
#include "WSA.h"
#include "Messages.h"

int main() {

	WSA::init();

	uint8 testData[3] = { 'a', 'b', 'c' };
	Packet testPacket(testData, sizeof(testData)/sizeof(testData[0]));

	//UDPSocket testSocket("localhost", DEFAULT_PORT);
	//testSocket.send(testPacket);

	TCPSocket tcpSocket("localhost", DEFAULT_PORT);
	tcpSocket.connect();
	tcpSocket.send(testPacket);
	tcpSocket.shutdown();

	bool receiving = true;
	Packet receivedPacket;
	do {
		receivedPacket = tcpSocket.receive();
	} while (receivedPacket.getMessageSize() > 0);

	system("pause");

	WSA::destroy();

	return 0;
}