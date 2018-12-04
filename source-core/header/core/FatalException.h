//@HIPE_LICENSE@
#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <core/osdependant.h>

class FatalException : public std::runtime_error
{

public:
	FatalException() : std::runtime_error("Unexpected execution")
	{

	}

	FatalException(const char * message);

	FatalException(std::string message);





};
