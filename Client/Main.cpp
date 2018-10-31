#include <iostream>

#include "UDPSocket.h"

#include <Windows.h>

int main() {

	// Initialize Winsock
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		throw std::runtime_error("Could not initialize Winsock.");
	}

	UDPSocket testSocket("localhost", DEFAULT_PORT);
	uint8 testData[3] = { 'a', 'b', 'c' };
	testSocket.send(testData, 3);

	system("pause");

	WSACleanup();

	return 0;
}