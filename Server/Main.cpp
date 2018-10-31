#include <iostream>

#include "UDPSocket.h"
#include "TCPSocket.h"
#include "ThreadPool.h"
#include "WSA.h"

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
			uint8 buffer[512];
			listenerSocket.receive(buffer, 512);
			std::cout << "received packet" << std::endl;
		}
	}, nullptr);

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	WSA::destroy();

	return 0;
}