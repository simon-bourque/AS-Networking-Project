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

	std::string m_seller;
	std::string m_highestBidder;

	uint64 m_auctionStartTime;

	static uint32 s_nextId;
public:
	Item(const std::string& description, float32 minimum, const std::string& seller, uint32 itemID);
	Item(const std::string& description, float32 minimum, const std::string& seller);
	virtual ~Item();

	uint32 getItemID() const { return m_itemID; }
	std::string getDescription() const { return m_description; }
	float32 getMinimum() const { return m_minimum; }
	float32 getCurrentHighest() const { return m_currentHighest; }
	std::string getSeller() const { return m_seller; }
	std::string getHighestBidder() const { return m_highestBidder; }

	void setCurrentHighest(float32 newBid) { m_currentHighest = newBid; }
	void setHighestBidder(const std::string& highestBidder) { m_highestBidder = highestBidder; }

	void setAuctionStartTime(uint64 time) { m_auctionStartTime = time; }
	uint64 getAuctionStartTime() const { return m_auctionStartTime; }

	static void setNextID(uint32 id) { s_nextId = id; }
};

