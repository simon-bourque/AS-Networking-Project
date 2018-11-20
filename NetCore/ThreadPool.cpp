#include "ThreadPool.h"

#include "Types.h"
#include <iostream>
#include <string>

ThreadPool* ThreadPool::s_instance = nullptr;

VOID CALLBACK MyWorkCallback(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work) {
	UNREFERENCED_PARAMETER(instance);
	UNREFERENCED_PARAMETER(work);

}

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

void ThreadPool::clean() {
	CloseThreadpoolCleanupGroupMembers(m_cleanUpGroup, FALSE, NULL);
}
