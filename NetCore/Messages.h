#pragma once

#include "Types.h"
#include "Packet.h"
#include <string>
#include <cassert>

enum class MessageType : uint8 {
	MSG_REGISTER,
	MSG_REGISTERED,
	MSG_UNREGISTERED,
	MSG_DEREGISTER,
	MSG_DEREG_CONF,
	MSG_DEREG_DENIED,
	MSG_OFFER,
	MSG_OFFER_CONF,
	MSG_NEW_ITEM,
	MSG_OFFER_DENIED,
	MSG_BID,
	MSG_HIGHEST,
	MSG_WIN,
	MSG_BID_OVER,
	MSG_SOLD_TO,
	MSG_NOT_SOLD
};

std::string messageTypeToString(MessageType msgType);

template<typename T>
Packet serializeMessage(const T& msg) {
	const uint32 size = sizeof(T);
	uint8 buffer[size];
	memcpy(buffer, &msg, size);

	return Packet(buffer, size);
}

template<typename T>
T deserializeMessage(const Packet& packet) {
	assert(packet.getMessageSize() == sizeof(T));

	T msg = *(reinterpret_cast<const T*>(packet.getMessageData()));

	return msg;
}

struct RegisterMessage {
	const MessageType type = MessageType::MSG_REGISTER;
	uint32 reqNum;
	char name[128];
	char iPAddress[128];
	char port[16];
};

struct RegisteredMessage {
	const MessageType type = MessageType::MSG_REGISTERED;
	uint32 reqNum;
	char name[128];
	char iPAddress[128];
	char port[16];
};