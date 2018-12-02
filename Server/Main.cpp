#include <iostream>
#include <mutex>

#include "Server.h"
#include "IPV4Address.h"

#include "Socket.h"
#include "ThreadPool.h"
#include "WSA.h"
#include "Log.h"
#include "Error.h"

#include <Windows.h>

std::mutex g_lock;

Server* g_Server = nullptr;

void init(const std::string& ip);
void shutdown();

BOOL WINAPI closeRoutine(_In_ DWORD ctrlType) {
	shutdown();
	return true;
}

int main() {
	// Set callback when console is closed
	SetConsoleCtrlHandler(closeRoutine, true);

	// Disable console selection
	HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (inputHandle != INVALID_HANDLE_VALUE) {
		DWORD mode = 0;
		GetConsoleMode(inputHandle, &mode);
		SetConsoleMode(inputHandle, mode & (~ENABLE_QUICK_EDIT_MODE));
	}

	std::cout << "Local ip: ";
	std::string ip;
	std::cin >> ip;

	log("[INFO] Initializing server...");
	init(ip);

	std::string cmd;
	do {
		std::cin >> cmd;
	} while (cmd != std::string("shutdown"));

	shutdown();

	return 0;
}

void init(const std::string& ip) {
	std::lock_guard<std::mutex> lock(g_lock);

	initErrorCodeStringMap();
	ThreadPool::init();
	WSA::init();

	g_Server = new Server(IPV4Address(ip, DEFAULT_PORT));
	g_Server->startUDPServiceThread();
	g_Server->startTCPServiceThread();
	g_Server->startConnectionServiceThread();
}

void shutdown() {
	std::lock_guard<std::mutex> lock(g_lock);

	if (g_Server != nullptr) {
		g_Server->shutdown();
		//ThreadPool::get()->clean();
		ThreadPool::destroy();
		WSA::destroy();
		delete g_Server;
	}
}