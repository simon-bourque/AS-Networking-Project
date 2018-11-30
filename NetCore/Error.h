#pragma once

#include "Types.h"
#include <string>

std::string getWindowsErrorString(uint32 error);
std::string getWSAErrorString(int32 error);
void initErrorCodeStringMap();