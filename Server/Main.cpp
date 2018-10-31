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
			MessageType type = static_cast<MessageType>(packet.getMessageData()[0]);
			std::cout << "[Receive] " << messageTypeToString(type) << " message" << std::endl;

			switch (type) {
			case MessageType::MSG_REGISTER:
				const RegisterMessage* msg = reinterpret_cast<const RegisterMessage*>(packet.getMessageData() + 1);
				std::cout << msg->reqNum << std::endl;
				std::cout << msg->name << std::endl;
				break;
			}
		}
	}, nullptr);

	//ThreadPool::get()->submit([](PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	//	UNREFERENCED_PARAMETER(work);

	//	CallbackMayRunLong(instance);

	//	// Create listen socket
	//	UDPSocket listenerSocket(nullptr, DEFAULT_PORT);
	//	listenerSocket.bind();

	//	std::cout << "Started listening..." << std::endl;
	//	bool listening = true;
	//	while (listening) {
	//		uint8 buffer[512];
	//		listenerSocket.receive(buffer, 512);
	//		std::cout << "received packet" << std::endl;
	//	}
	//}, nullptr);

	// Using main thread as listen thread
	TCPSocket listenerSocket(nullptr, DEFAULT_PORT);
	listenerSocket.bind();

	std::cout << "Started listening..." << std::endl;
	listenerSocket.listen();

	bool listening = true;
	while (listening) {
		TCPSocket clientSocket = listenerSocket.accept();
		std::cout << "Accepted connection..." << std::endl;

		ThreadPool::get()->submit([](PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
			UNREFERENCED_PARAMETER(work);

			CallbackMayRunLong(instance);

			// Receive data from connection until client shuts down connection
			// https://docs.microsoft.com/en-us/windows/desktop/winsock/receiving-and-sending-data-on-the-server
			
		}, nullptr);
	}

	ThreadPool::get()->clean();
	ThreadPool::destroy();
	WSA::destroy();

	return 0;
}