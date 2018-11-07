#pragma once

#include "Messages.h"

class IPV4Address;

class Log {
public:
	enum class LogType {
		LOG_SEND,
		LOG_RECEIVE
	};
private:
	Log();
	virtual ~Log();
public:
	static void log(LogType logType, MessageType msgType, const IPV4Address& address);
};

