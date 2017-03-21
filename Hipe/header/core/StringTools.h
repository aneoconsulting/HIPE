#pragma once

#include <iostream> 
#include <sstream>   
#include <iomanip>
#include <core/misc.h>

long ToHex(const std::string& s, bool upper_case = true);

std::string ToString(long n);
