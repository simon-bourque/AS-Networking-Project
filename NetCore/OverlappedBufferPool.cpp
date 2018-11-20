#include "OverlappedBufferPool.h"



OverlappedBufferPool::OverlappedBufferPool()
{
}


OverlappedBufferPool::~OverlappedBufferPool()
{
}

OverlappedBufferHandle OverlappedBufferPool::requestOverlappedBuffer(void* param) {
	OverlappedBufferHandle handle = 0;
	if (!m_availableOverlappedBuffers.empty()) {
		handle = m_availableOverlappedBuffers.front();
		m_availableOverlappedBuffers.pop();
	}
	else {
		m_overlappedBuffers.emplace_back();
		handle = m_overlappedBuffers.size();
	}
	
	return handle;
}

void OverlappedBufferPool::releaseOverlappedBuffer(OverlappedBufferHandle handle) {
	m_availableOverlappedBuffers.push(handle);
}

OverlappedBuffer& OverlappedBufferPool::getOverlappedBuffer(OverlappedBufferHandle handle) {
	return m_overlappedBuffers[handle - 1];
}