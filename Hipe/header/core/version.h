#pragma once

//Version where the source code start changes

#define MAJOR_VERSION 0

#define MINOR_VERSION 1

//The next version of the code will land after validation
#define NEXT_MAJOR 0

#define NEXT_MINOR 2
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <string>
#include <core/core_export.h>

std::string getVersion(); 
std::string getHash();
