#include "HipeException.h"

HipeException::HipeException(const char * m) : message(m)
{
}

HipeException::HipeException(std::string m) : message(m)
{
}


const char* HipeException::what() const noexcept
{
	return this->message.c_str();
}
