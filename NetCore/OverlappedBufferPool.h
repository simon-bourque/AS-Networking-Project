#pragma once

#include "Types.h"
#include "OverlappedBuffer.h"

#include <vector>
#include <queue>

using OverlappedBufferHandle = uint32;

class OverlappedBufferPool {
private:
	std::vector<OverlappedBuffer> m_overlappedBuffers;
	std::queue<OverlappedBufferHandle> m_availableOverlappedBuffers;
public:
	OverlappedBufferPool();
	virtual ~OverlappedBufferPool();

	OverlappedBufferHandle requestOverlappedBuffer(void* param);
	void releaseOverlappedBuffer(OverlappedBufferHandle handle);

	OverlappedBuffer& getOverlappedBuffer(OverlappedBufferHandle handle);

	static OverlappedBufferPool* get() {
		static OverlappedBufferPool instance;
		return &instance;
	}
};

