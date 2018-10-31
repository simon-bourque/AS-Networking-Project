#include "WSA.h"

#include <Windows.h>

#include <stdexcept>

WSA* WSA::s_instance = nullptr;

WSA::WSA() {
	// Initialize Winsock
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		throw std::runtime_error("Could not initialize Winsock.");
	}
}


WSA::~WSA() {
	WSACleanup();
}
