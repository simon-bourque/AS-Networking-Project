#include <iostream>

#include "Client.h"
#include "WSA.h"

int main() {

	WSA::init();

	Client client("Oh my, this frozen pizza is so good! Where is it from? It's not delivery, it's Garbagio");

	client.sendRegister("127.0.0.1", DEFAULT_PORT);

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