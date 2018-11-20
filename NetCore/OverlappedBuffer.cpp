#include "OverlappedBuffer.h"



OverlappedBuffer::OverlappedBuffer() : m_flags(0) {
	ZeroMemory(&m_overlapped, sizeof(m_overlapped));
	m_overlapped.hEvent = nullptr;

	m_buffer.buf = new char[OVERLAPPED_BUFFER_SIZE];
	m_buffer.len = OVERLAPPED_BUFFER_SIZE;

	m_senderAddressSize = sizeof(m_senderAddress);
	ZeroMemory(&m_senderAddress, m_senderAddressSize);
}


OverlappedBuffer::~OverlappedBuffer() {
	delete[] m_buffer.buf;
}

uint8* OverlappedBuffer::getData() {
	return reinterpret_cast<uint8*>(m_buffer.buf);
}