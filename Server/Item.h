#pragma once

#include "Types.h"
#include <string>
#include "IPV4Address.h"

class Item {
private:
	uint32 m_itemID;

	std::string m_description;
	float32 m_minimum;
	float32 m_currentHighest;

	IPV4Address m_seller;
	IPV4Address m_highestBidder;
public:
	Item(const std::string& description, float32 minimum, const IPV4Address& seller);
	virtual ~Item();

	uint32 getItemID() const { return m_itemID; }
	std::string getDescription() const { return m_description; }
	float32 getMinimum() const { return m_minimum; }
	float32 getCurrentHighest() const { return m_currentHighest; }
	IPV4Address getSeller() const { return m_seller; }
	IPV4Address getHighestBidder() const { return m_highestBidder; }

	void setCurrentHighest(float32 newBid) { m_currentHighest = newBid; }
	void setHighestBidder(const IPV4Address& highestBidder) { m_highestBidder = highestBidder; }
};

