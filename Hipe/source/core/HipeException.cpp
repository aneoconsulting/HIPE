#include "HipeException.h"

HipeException::HipeException(const char * m) : std::runtime_error(m)
{
	
}


HipeException::HipeException(HipeException & ex) : std::runtime_error(ex.what())
{
	
}

HipeException::HipeException(std::string m) : std::runtime_error(m)
{

}

