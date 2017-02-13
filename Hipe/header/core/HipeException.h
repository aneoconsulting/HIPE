#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <core/osdependant.h>

class HipeException : public std::runtime_error
{	

	public:
		HipeException() : std::runtime_error("Unexpected execution")
		{
		
		}

		HipeException(const char * message);

		HipeException(std::string message);



	

};
