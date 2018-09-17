//@HIPE_LICENSE@
#include "FatalException.h"

FatalException::FatalException(const char * m) : std::runtime_error(m)
{

}


FatalException::FatalException(std::string m) : std::runtime_error(m)
{

}

