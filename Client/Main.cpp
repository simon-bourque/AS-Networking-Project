#include <iostream>

#include "Client.h"
#include "WSA.h"

int main() {
	WSA::init();

	// Creating client
	{
		Client client("127.0.0.1", DEFAULT_PORT);
	}

	WSA::destroy();

	return 0;
}