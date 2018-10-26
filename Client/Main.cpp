#include <iostream>

#include "SocketManager.h"
#include "UDPSocket.h"

int main() {
	SocketManager::init();

	//SocketManager::get()->createTCPSocket();

	UDPSocket testSocket("localhost", DEFAULT_PORT);
	uint8 testData[3] = { 'a', 'b', 'c' };
	testSocket.send(testData, 3);

	system("pause");

	SocketManager::destroy();

	return 0;
}