#pragma once

#include <Windows.h>

typedef VOID (CALLBACK *ThreadExecutionFunc)(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

class ThreadPool {
private:
	static ThreadPool* s_instance;

	PTP_POOL m_pool;
	TP_CALLBACK_ENVIRON m_callbackEnvironment;
	PTP_CLEANUP_GROUP m_cleanUpGroup;
	
	ThreadPool();
public:
	virtual ~ThreadPool();

	PTP_WORK submit(ThreadExecutionFunc func, void* ptr);
	void clean();

	static void init() { s_instance = new ThreadPool(); }
	static void destroy() { delete s_instance; }
	static ThreadPool* get() { return s_instance; }
};

