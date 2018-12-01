#include <iostream>

#include "Client.h"
#include "WSA.h"

int main() {
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