#include "Messages.h"

std::string messageTypeToString(MessageType msgType) {
	switch (msgType) {
	case MessageType::MSG_REGISTER:
		return "REGISTER";
	case MessageType::MSG_REGISTERED:
		return "REGISTERED";
	case MessageType::MSG_UNREGISTERED:
		return "UNREGISTERED";
	case MessageType::MSG_DEREGISTER:
		return "DEREGISTER";
	case MessageType::MSG_DEREG_CONF:
		return "DEREG_CONF";
	case MessageType::MSG_DEREG_DENIED:
		return "DEREG_DENIED";
	case MessageType::MSG_OFFER:
		return "OFFER";
	case MessageType::MSG_OFFER_CONF:
		return "OFFER_CONF";
	case MessageType::MSG_NEW_ITEM:
		return "NEW_ITEM";
	case MessageType::MSG_OFFER_DENIED:
		return "OFFER_DENIED";
	case MessageType::MSG_BID:
		return "BID";
	case MessageType::MSG_HIGHEST:
		return "HIGHEST";
	case MessageType::MSG_WIN:
		return "WIN";
	case MessageType::MSG_BID_OVER:
		return "BID_OVER";
	case MessageType::MSG_SOLD_TO:
		return "SOLD_TO";
	case MessageType::MSG_NOT_SOLD:
		return "NOT_SOLD";
	default:
		return "UNKNOWN";
	}
}