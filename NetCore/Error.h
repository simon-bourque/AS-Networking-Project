#pragma once

#include "Types.h"
#include <string>

std::string getWSAErrorString(int32 error);
void initErrorCodeStringMap();