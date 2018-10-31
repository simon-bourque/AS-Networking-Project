#include <iostream>
#include <unordered_map>

#include "UDPSocket.h"
#include "TCPSocket.h"
#include "ThreadPool.h"
#include "WSA.h"
#include "Connection.h"
#include "Messages.h"

std::unordered_map<std::string, Connection> g_connections;

int main() {
	std::cout << "Initializing server..." << std::endl;

	ThreadPool::init();
	WSA::init();

	// Create listen thread
	ThreadPool::get()->submit([](PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
		UNREFERENCED_PARAMETER(work);

		CallbackMayRunLong(instance);

		// Create listen socket
		UDPSocket listenerSocket(nullptr, DEFAULT_PORT);
		listenerSocket.bind();

		std::cout << "Started listening..." << std::endl;
		bool listening = true;
		while (listening) {
			Packet packet = listenerSocket.receive();

			std::cout << "[Receive] " << messageTypeToString(static_cast<MessageType>(packet.getMessageData()[0])) << " message" << std::endl;
		}
	}, nullptr);

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	WSA::destroy();

	return 0;
}