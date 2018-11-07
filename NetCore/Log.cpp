#include "Log.h"

#include <iostream>

void log(LogType logType, MessageType msgType, const IPV4Address& address) {
	switch (logType) {
	case LogType::LOG_SEND:
		std::cout << "[Send : ";
		break;
	case LogType::LOG_RECEIVE:
		std::cout << "[Receive : ";
		break;
	}
	
	std::cout << address.getSocketAddressAsString() << "] " << messageTypeToString(msgType) << std::endl;
}

void log(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}