#pragma once

#include <string>
#include <Winsock2.h>

#include "Types.h"

class IPV4Address {
private:
	sockaddr_in m_address;
public:
	IPV4Address();
	IPV4Address(const std::string& address, const std::string& port);
	IPV4Address(const sockaddr_in& address);
	virtual ~IPV4Address();

	const sockaddr* getSocketAddress() const { return reinterpret_cast<const sockaddr*>(&m_address); }
	uint32 getSocketAddressSize() const { return sizeof(sockaddr_in); }

	std::string getSocketAddressAsString() const;
	std::string getSocketPortAsString() const;
};

