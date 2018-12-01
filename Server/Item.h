#pragma once

#include "Types.h"
#include <string>

class Item {
private:
	uint32 m_itemID;

	std::string m_description;
	float32 m_minimum;
public:
	Item(const std::string& description, float32 minimum);
	virtual ~Item();

	uint32 getItemID() const { return m_itemID; }
	std::string getDescription() const { return m_description; }
	float32 getMinimum() const { return m_minimum; }
};

