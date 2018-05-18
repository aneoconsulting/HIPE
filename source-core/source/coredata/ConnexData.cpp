//@HIPE_LICENSE@
#include <coredata/ConnexData.h>
#include "core/HipeException.h"

namespace data
{
	template <>
	double* CopyObject<double *>::copy(double* &left)
	{
		throw HipeException("Can't allocate native array. need the size to do it");
	}

	WayData ConnexDataBase::getWay() const
	{
		return _way;
	}

	ConnexDataBase& ConnexDataBase::getCast()
	{
		return *this;
	}

	ConnexDataBase& ConnexDataBase::operator<<(ConnexDataBase& right)
	{
		this->getCast().operator<<(right.getCast());

		return *this;
	}
}
