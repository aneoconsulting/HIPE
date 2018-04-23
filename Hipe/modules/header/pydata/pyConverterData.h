//@HIPE_LICENSE@
#pragma once
#include <pydata/pyData.h>

#include <pydata/pydata_export.h>
#include <vector>

#pragma warning(push, 0)
#include <boost/python.hpp>
#pragma warning(pop)

namespace pydata
{
	// Converts a C++ vector to a python list
template <class T>
boost::python::list toPythonList(std::vector<T> vector) {
	typename std::vector<T>::iterator iter;
	boost::python::list list;
	for (iter = vector.begin(); iter != vector.end(); ++iter) {
		list.append(*iter);
	}
	return list;
}


	class PYDATA_EXPORT pyDataConverter
	{
		template <typename DataType>
		static pyData* convertTo(DataType& dataIn)
		{
			return nullptr;
		}
	};
}
