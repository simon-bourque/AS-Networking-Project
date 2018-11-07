#pragma once

#include "Types.h"

#include "IPV4Address.h"

class Packet {
public:
	const static uint32 PACKET_SIZE;
private:
	uint8* m_buffer;
	uint32 m_messageSize;

	IPV4Address m_address;
public:
	Packet();
	Packet(uint8* buffer, uint32 buffSize);
	Packet(const Packet& packet);
	Packet(Packet&& packet);
	virtual ~Packet();

	Packet& operator=(const Packet& packet);
	Packet& operator=(Packet&& packet);

	const uint8* getMessageData() const { return m_buffer; }
	uint32 getMessageSize() const { return m_messageSize; }

	// Really only for UDP more than anything
	void setAddress(const IPV4Address& address) { m_address = address; }
	const IPV4Address& getAddress() const { return m_address; }
};

