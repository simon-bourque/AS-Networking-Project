#include <iostream>

#include "Client.h"
#include "WSA.h"

int main() {
	HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (inputHandle != INVALID_HANDLE_VALUE) {
		DWORD mode = 0;
		GetConsoleMode(inputHandle, &mode);
		SetConsoleMode(inputHandle, mode & (~ENABLE_QUICK_EDIT_MODE));
	}

	WSA::init();

	std::string ip;
	std::cout << "Server IP: " << std::endl;
	std::cin >> ip;
	std::cout << std::endl;

	// Creating client
	{
		Client client(ip, DEFAULT_PORT);
	}

	WSA::destroy();

	return 0;
}