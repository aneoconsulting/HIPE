#pragma once
#include <opencv2/core.hpp>

#if defined ( WIN32 )
#  define __func__ __FUNCTION__
#endif

class _NotYetImpelmentedException : cv::Exception
{

public:
	_NotYetImpelmentedException(cv::String errString, cv::String function, cv::String file, int numline) :
		Exception(1, errString, function, file, numline)
	{

	}
};


#define NotYetImplementedException(errString) _NotYetImpelmentedException(errString, __func__, __FILE__, __LINE__)