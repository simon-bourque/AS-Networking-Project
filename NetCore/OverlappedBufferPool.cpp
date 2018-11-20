#include "OverlappedBufferPool.h"

#include <mutex>

static std::mutex g_lock;

OverlappedBufferPool::OverlappedBufferPool()
{
}


OverlappedBufferPool::~OverlappedBufferPool()
{
}

OverlappedBufferHandle OverlappedBufferPool::requestOverlappedBuffer(void* param) {
	std::lock_guard<std::mutex> lock(g_lock);

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
	std::lock_guard<std::mutex> lock(g_lock);

	m_availableOverlappedBuffers.push(handle);
}

OverlappedBuffer& OverlappedBufferPool::getOverlappedBuffer(OverlappedBufferHandle handle) {
	std::lock_guard<std::mutex> lock(g_lock);

	return m_overlappedBuffers[handle - 1];
}