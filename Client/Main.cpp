#include <iostream>

#include "SocketManager.h"

int main() {

	SocketManager::init();

	system("pause");

	SocketManager::destroy();

	return 0;
}