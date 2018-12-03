#include "Item.h"


uint32 Item::s_nextId = 1;

Item::Item(const std::string& description, float32 minimum, const std::string& seller) :
	m_description(description)
	, m_minimum(minimum)
	, m_currentHighest(minimum)
	, m_seller(seller)
	, m_auctionStartTime(0)
{
	m_itemID = s_nextId++;
}

Item::Item(const std::string& description, float32 minimum, const std::string& seller, uint32 itemID) :
	m_description(description)
	, m_minimum(minimum)
	, m_currentHighest(minimum)
	, m_seller(seller)
	, m_auctionStartTime(0)
	, m_itemID(itemID)
{}


Item::~Item() {}
