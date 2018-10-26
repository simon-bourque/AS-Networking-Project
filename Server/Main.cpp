#include <iostream>

#include "SocketManager.h"
#include "ThreadPool.h"
#include "UDPSocket.h"

int main() {
	std::cout << "Initializing server..." << std::endl;

	SocketManager::init();
	ThreadPool::init();

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
			uint8 buffer[512];
			listenerSocket.recieve(buffer, 512);
			std::cout << "Recieved packet" << std::endl;
		}
	}, nullptr);

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	SocketManager::destroy();
	return 0;
}