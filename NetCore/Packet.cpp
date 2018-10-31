#include "Packet.h"

#include <cstring>
#include <algorithm>

const uint32 Packet::PACKET_SIZE = 512;

Packet::Packet() : m_messageSize(0) {
	m_buffer = new uint8[PACKET_SIZE];
}

Packet::Packet(uint8* buffer, uint32 buffSize) : Packet() {
	m_messageSize = std::min(buffSize, PACKET_SIZE);
	memcpy(m_buffer, buffer, m_messageSize);
}

Packet::Packet(const Packet& packet) {
	m_buffer = new uint8[PACKET_SIZE];
	m_messageSize = packet.m_messageSize;

	memcpy(m_buffer, packet.m_buffer, m_messageSize);
}

Packet::Packet(Packet&& packet) {
	m_buffer = packet.m_buffer;
	m_messageSize = packet.m_messageSize;

	packet.m_buffer = nullptr;
}

Packet::~Packet() {
	delete[] m_buffer;
}

Packet& Packet::operator=(const Packet& packet) {
	m_messageSize = packet.m_messageSize;
	memcpy(m_buffer, packet.m_buffer, m_messageSize);
	return *this;
}

Packet& Packet::operator=(Packet&& packet) {
	m_buffer = packet.m_buffer;
	m_messageSize = packet.m_messageSize;

	packet.m_buffer = nullptr;
	return *this;
}