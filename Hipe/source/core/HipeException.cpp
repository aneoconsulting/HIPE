//@HIPE_LICENSE@
#include "HipeException.h"

HipeException::HipeException(const char * m) : std::runtime_error(m)
{
	
}


HipeException::HipeException(std::string m) : std::runtime_error(m)
{

}

