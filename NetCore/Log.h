#pragma once

#include "Messages.h"

class IPV4Address;

enum class LogType {
	LOG_SEND,
	LOG_RECEIVE
};

void log(LogType logType, MessageType msgType, const IPV4Address& address);
void log(const char* format, ...);

