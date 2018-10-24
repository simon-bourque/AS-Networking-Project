#include <iostream>

#include "ThreadPool.h"

int main() {
	std::cout << "Initializing server..." << std::endl;

	ThreadPool::init();

	// Create listen thread
	ThreadPool::get()->submit([](PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
		UNREFERENCED_PARAMETER(work);

		CallbackMayRunLong(instance);

		bool listening = true;
		while (listening) {
			
		}
	}, nullptr);
	
	ThreadPool::destroy();
	return 0;
}