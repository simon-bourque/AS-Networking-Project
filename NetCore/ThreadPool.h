#pragma once

#include <Winsock2.h>
#include <Windows.h>
#include "Types.h"

typedef VOID (CALLBACK *ThreadExecutionFunc)(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work);

typedef VOID (CALLBACK *TimerCallback)(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_TIMER timer);


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
	PTP_TIMER submitTimer(TimerCallback func, void* ptr, uint64 auctionTime = 3000000000ull);
	void clean();

	static void init() { s_instance = new ThreadPool(); }
	static void destroy() { delete s_instance; }
	static ThreadPool* get() { return s_instance; }
};

