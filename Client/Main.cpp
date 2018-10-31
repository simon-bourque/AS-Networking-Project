#include <iostream>

#include "UDPSocket.h"
#include "WSA.h"

int main() {

	WSA::init();

	UDPSocket testSocket("localhost", DEFAULT_PORT);
	uint8 testData[3] = { 'a', 'b', 'c' };
	testSocket.send(Packet(testData, 3));

	system("pause");

	WSA::destroy();

	return 0;
}