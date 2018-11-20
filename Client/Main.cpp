#include <iostream>

#include "Client.h"
#include "WSA.h"

int main() {
	WSA::init();

	// Creating client
	Client client;

	WSA::destroy();

	return 0;
}