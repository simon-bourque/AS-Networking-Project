#include <iostream>

#include "UDPSocket.h"
#include "WSA.h"
#include "Messages.h"

int main() {

	WSA::init();

	UDPSocket testSocket("localhost", DEFAULT_PORT);
	MessageType type = MessageType::MSG_REGISTER;
	testSocket.send(Packet(reinterpret_cast<uint8*>(&type), 1));

	system("pause");

	WSA::destroy();

	return 0;
}