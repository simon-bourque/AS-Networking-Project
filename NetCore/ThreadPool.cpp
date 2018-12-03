#include "ThreadPool.h"

#include <iostream>
#include <string>

ThreadPool* ThreadPool::s_instance = nullptr;

ThreadPool::ThreadPool() {
	m_pool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(m_pool, 300);
	SetThreadpoolThreadMaximum(m_pool, 500);

	InitializeThreadpoolEnvironment(&m_callbackEnvironment);

	m_cleanUpGroup = CreateThreadpoolCleanupGroup();

	SetThreadpoolCallbackPool(&m_callbackEnvironment, m_pool);
	SetThreadpoolCallbackCleanupGroup(&m_callbackEnvironment, m_cleanUpGroup, NULL);
}

ThreadPool::~ThreadPool() {
	CloseThreadpoolCleanupGroupMembers(m_cleanUpGroup, TRUE, NULL);
	CloseThreadpoolCleanupGroup(m_cleanUpGroup);
	CloseThreadpool(m_pool);
	DestroyThreadpoolEnvironment(&m_callbackEnvironment);
}

PTP_WORK ThreadPool::submit(ThreadExecutionFunc func, void* ptr) {
	PTP_WORK workObject = CreateThreadpoolWork(func, ptr, &m_callbackEnvironment);

	SubmitThreadpoolWork(workObject);

	return workObject;
}

PTP_TIMER ThreadPool::submitTimer(TimerCallback func, void* ptr, uint64 auctionTime) {
	PTP_TIMER timerObject = CreateThreadpoolTimer(func, ptr, &m_callbackEnvironment);

	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	ULARGE_INTEGER* time = reinterpret_cast<ULARGE_INTEGER*>(&fileTime);
	time->QuadPart += auctionTime;

	SetThreadpoolTimer(timerObject, &fileTime, 0, 0);

	return timerObject;
}

void ThreadPool::clean() {
	CloseThreadpoolCleanupGroupMembers(m_cleanUpGroup, FALSE, NULL);
}
