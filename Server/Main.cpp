#include <iostream>
#include <unordered_map>

#include "Server.h"
#include "IPV4Address.h"

#include "UDPSocket.h"
#include "TCPSocket.h"
#include "ThreadPool.h"
#include "WSA.h"
#include "Connection.h"
#include "Messages.h"
#include "Log.h"

std::unordered_map<std::string, Connection> g_connections;

int main() {
	std::cout << "Initializing server..." << std::endl;

	ThreadPool::init();
	WSA::init();

	Server server(IPV4Address("127.0.0.1", DEFAULT_PORT));
	server.startUDPServiceThread();
	server.startTCPServiceThread();

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	WSA::destroy();

	return 0;
}