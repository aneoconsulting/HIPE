#pragma once

#include <iostream> 
#include <sstream>   
#include <iomanip>
#include <core/HipeNativeTypes.h>
#include <core/misc.h>

hUInt64 ToHex(const std::string& s, bool upper_case = true);

std::string ToString(hUInt64 n);
