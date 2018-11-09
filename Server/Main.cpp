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

	//// Using main thread as listen thread
	//TCPSocket listenerSocket;
	//IPV4Address address("127.0.0.1", DEFAULT_PORT);
	//listenerSocket.bind(address);
	//
	//std::cout << "Started listening..." << std::endl;
	//listenerSocket.listen();
	//
	//bool listening = true;
	//while (listening) {
	//	TCPSocket clientSocket = listenerSocket.accept();
	//	std::cout << "Accepted connection..." << std::endl;
	//
	//	// Check if connection exists and then set state to connected
	//	IPV4Address peerAddress = clientSocket.getPeerAddress();
	//	std::cout << peerAddress.getSocketAddressAsString() << std::endl;
	//	auto connectionIter = g_connections.find(peerAddress.getSocketAddressAsString());
	//	if (connectionIter != g_connections.end()) {
	//
	//	}
	//}

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	WSA::destroy();

	return 0;
}