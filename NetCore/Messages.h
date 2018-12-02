#pragma once

#include "Types.h"
#include "Packet.h"
#include <string>
#include <cassert>

// Field lengths
static constexpr uint32 NAMELENGTH = 128;
static constexpr uint32 IPLENGTH = 128;
static constexpr uint32 REASONLENGTH = 128;
static constexpr uint32 DESCLENGTH = 128;
static constexpr uint32 PORTLENGTH = 16;

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
	char name[NAMELENGTH];
	char iPAddress[IPLENGTH];
	char port[PORTLENGTH];
};

struct RegisteredMessage {
	const MessageType type = MessageType::MSG_REGISTERED;
	uint32 reqNum;
	char name[NAMELENGTH];
	char iPAddress[IPLENGTH];
	char port[PORTLENGTH];
};

struct UnregisteredMessage {
	const MessageType type = MessageType::MSG_UNREGISTERED;
	uint32 reqNum;
	char reason[REASONLENGTH];
};

struct DeregisterMessage {
	const MessageType type = MessageType::MSG_DEREGISTER;
	uint32 reqNum;
	char name[NAMELENGTH];
	char iPAddress[IPLENGTH];
};

struct DeregConfMessage {
	const MessageType type = MessageType::MSG_DEREG_CONF;
	uint32 reqNum;
};

struct DeregDeniedMessage {
	const MessageType type = MessageType::MSG_DEREG_DENIED;
	uint32 reqNum;
	char reason[REASONLENGTH];
};

struct OfferMessage {
	const MessageType type = MessageType::MSG_OFFER;
	uint32 reqNum;
	char name[NAMELENGTH];
	char iPAddress[IPLENGTH];
	char description[DESCLENGTH];
	float32 minimum;
};

struct OfferConfMessage {
	const MessageType type = MessageType::MSG_OFFER_CONF;
	uint32 reqNum;
	uint32 itemNum;
	char description[DESCLENGTH];
	float32 minimum;
};

struct NewItemMessage {
	const MessageType type = MessageType::MSG_NEW_ITEM;
	uint32 itemNum;
	char description[DESCLENGTH];
	float32 minimum;
	char port[PORTLENGTH]; // Maybe we don't need this?
};

struct OfferDeniedMessage {
	const MessageType type = MessageType::MSG_OFFER_DENIED;
	uint32 reqNum;
	char reason[REASONLENGTH];
};

struct BidMessage {
	const MessageType type = MessageType::MSG_BID;
	uint32 reqNum;
	uint32 itemNum;
	float32 amount;
};

struct HighestMessage {
	const MessageType type = MessageType::MSG_HIGHEST;
	uint32 itemNum;
	float32 amount;
	char description[DESCLENGTH];
};

struct WinMessage {
	const MessageType type = MessageType::MSG_WIN;
	uint32 itemNum;
	char name[NAMELENGTH];
	char iPAddress[IPLENGTH];
	char port[PORTLENGTH];
	float32 amount;
};

struct BidOverMessage {
	const MessageType type = MessageType::MSG_BID_OVER;
	uint32 itemNum;
	float32 amount;
};

struct SoldToMessage {
	const MessageType type = MessageType::MSG_SOLD_TO;
	uint32 itemNum;
	char name[NAMELENGTH];
	char iPAddress[IPLENGTH];
	char port[PORTLENGTH];
	float32 amount;
};

struct NotSoldMessage {
	const MessageType type = MessageType::MSG_NOT_SOLD;
	uint32 itemNum;
	char reason[REASONLENGTH];
};