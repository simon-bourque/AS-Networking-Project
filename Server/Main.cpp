#include <iostream>

#include "SocketManager.h"
#include "ThreadPool.h"

int main() {
	std::cout << "Initializing server..." << std::endl;

	SocketManager::init();
	ThreadPool::init();

	// Create listen thread
	ThreadPool::get()->submit([](PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
		UNREFERENCED_PARAMETER(work);

		CallbackMayRunLong(instance);

		// Create listen socket
		Socket listenConnectionSocket = SocketManager::get()->createUDPSocket();
		listenConnectionSocket.bind();

		std::cout << "Started listening,.." << std::endl;
		bool listening = true;
		while (listening) {
			
		}
	}, nullptr);

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	SocketManager::destroy();
	return 0;
}