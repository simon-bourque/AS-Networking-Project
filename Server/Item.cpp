#include "Item.h"



Item::Item(const std::string& description, float32 minimum, const IPV4Address& seller) :
	m_description(description)
	, m_minimum(minimum)
	, m_currentHighest(minimum)
	, m_seller(seller)
{
	static uint32 id = 1;
	m_itemID = id++;
}


Item::~Item() {}
