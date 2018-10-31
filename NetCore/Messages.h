#pragma once

#include "Types.h"
#include <string>

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

struct RegisterMessage {
	uint32 reqNum;
	char name[128];
	char iPAddress[128];
	char port[16];
};

struct RegisteredMessage {
	uint32 reqNum;
	char name[128];
	char iPAddress[128];
	char port[16];
};