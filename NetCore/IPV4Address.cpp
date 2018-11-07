#include "IPV4Address.h"

#include "Types.h"

#include <ws2tcpip.h>

IPV4Address::IPV4Address() {
	ZeroMemory(&m_address, sizeof(m_address));
}

IPV4Address::IPV4Address(const std::string& address, const std::string& port) {
	ZeroMemory(&m_address, sizeof(m_address));
	m_address.sin_family = AF_INET;
	
	// Convert port number
	int32 portNum = stoi(port);
	m_address.sin_port = htons(portNum);

	// Convert ip
	int32 error = InetPton(AF_INET, address.c_str(), &m_address.sin_addr);
	if (error != 1) {
		// TODO go back and look at docs for this function because error can be either 0 or -1
		// error occured
	}
}

IPV4Address::IPV4Address(const sockaddr_in& address) : m_address(address) {}

IPV4Address::~IPV4Address() {}

std::string IPV4Address::getSocketAddressAsString() const {
	constexpr uint32 bufferSize = 128;
	char buffer[bufferSize];
	PCTSTR error = InetNtop(AF_INET, &m_address.sin_addr, buffer, bufferSize);
	if (error == nullptr) {
		// TODO go back and look at docs for this function
		// error occured
	}

	return std::string(buffer);
}

std::string IPV4Address::getSocketPortAsString() const {
	uint32 portNum = ntohs(m_address.sin_port);
	return std::to_string(portNum);
}