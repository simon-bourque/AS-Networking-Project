#include <iostream>

#include "Client.h"
#include "WSA.h"

int main() {

	WSA::init();

	uint8 testData[3] = { 'a', 'b', 'c' };
	Packet testPacket(testData, sizeof(testData)/sizeof(testData[0]));

	Client client("Oh my, this frozen pizza is so good! Where is it from? It's not delivery, it's Garbagio");

	client.sendRegister("127.0.0.1", DEFAULT_PORT);
	client.connect();
	client.sendPacket(testPacket);
	client.shutdown();

	system("pause");

	WSA::destroy();

	return 0;
}