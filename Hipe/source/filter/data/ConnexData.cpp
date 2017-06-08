#include <filter/data/ConnexData.h>
#include "core/HipeException.h"

namespace filter
{
	namespace data
	{
		template <>
		double* CopyObject<double *>::copy(double* &left)
		{
			throw HipeException("Can't allocate native array. need the size to do it");
		}

		
	}
}
