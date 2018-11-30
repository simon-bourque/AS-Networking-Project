#include <iostream>

#include "Server.h"
#include "IPV4Address.h"

#include "Socket.h"
#include "ThreadPool.h"
#include "WSA.h"
#include "Log.h"
#include "Error.h"

#include <Windows.h>

Server* g_Server = nullptr;

BOOL WINAPI closeRoutine(_In_ DWORD ctrlType) {
	g_Server->shutdown();
	std::cout << "Step 1" << std::endl;
	ThreadPool::get()->clean();
	std::cout << "Step 2" << std::endl;
	Sleep(INFINITE);
	ThreadPool::get()->destroy();
	std::cout << "Step 3" << std::endl;
	WSA::destroy();
	std::cout << "Step 4" << std::endl;
	delete g_Server;
	std::cout << "Done" << std::endl;

	return true;
}

int main() {
	SetConsoleCtrlHandler(closeRoutine, true);

	std::cout << "Local ip: ";
	std::string ip;
	std::cin >> ip;

	log("[INFO] Initializing server...");

	initErrorCodeStringMap();
	ThreadPool::init();
	WSA::init();

	g_Server = new Server(IPV4Address(ip, DEFAULT_PORT));
	g_Server->startUDPServiceThread();
	g_Server->startTCPServiceThread();
	g_Server->startConnectionServiceThread();

	//Console::get()->run();

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	WSA::destroy();
	delete g_Server;

	std::cout << "Done" << std::endl;

	return 0;
}