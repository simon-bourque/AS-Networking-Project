#include <iostream>

#include "Server.h"
#include "IPV4Address.h"

#include "Socket.h"
#include "ThreadPool.h"
#include "WSA.h"
#include "Log.h"

int main() {
	std::cout << "Local ip: ";
	std::string ip;
	std::cin >> ip;

	log("[INFO] Initializing server...");

	ThreadPool::init();
	WSA::init();

	Server server(IPV4Address(ip, DEFAULT_PORT));
	server.startUDPServiceThread();
	server.startTCPServiceThread();

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	WSA::destroy();

	return 0;
}