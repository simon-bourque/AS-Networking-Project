#include "Log.h"

#include <iostream>

Log::Log() {}

Log::~Log() {}

void Log::log(LogType logType, MessageType msgType, const IPV4Address& address) {
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