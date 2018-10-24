#include <iostream>

#include "SocketManager.h"

int main() {
	SocketManager::init();

	SocketManager::get()->createTCPSocket();

	system("pause");

	SocketManager::destroy();

	return 0;
}