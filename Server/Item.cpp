#include "Item.h"



Item::Item(const std::string& description, float32 minimum) :
	m_description(description)
	, m_minimum(minimum)
{
	static uint32 id = 0;
	m_itemID = id++;
}


Item::~Item() {}
