#pragma once

#include <iostream> 
#include <sstream>   
#include <iomanip>
#include <core/HipeNativeTypes.h>
#include <core/misc.h>

hUInt64 ToHex(const std::string& s, bool upper_case = true);

std::string ToString(hUInt64 n);


std::string removeDirectoryName(std::string pathname);

std::string extractDirectoryName(const std::string& str);

std::string removeFileExtension(const std::string&str);

std::string extractFileName(const std::string&str);