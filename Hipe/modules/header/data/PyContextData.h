//@HIPE_LICENSE@
#pragma once
#include <coredata/IODataType.h>

#include <coredata/IOData.h>
#include <coredata/Data.h>

#include <coredata/data_export.h>

#include <atomic>

#pragma warning(push, 0) 
#include <boost/python.hpp>
#pragma warning(pop)

namespace data
{
	/**
	 * \brief PyContextData is the data type used to handle an image. Uses OpenCV.
	 */
	class DATA_EXPORT PyContextData : public IOData<Data, PyContextData>
	{
	public:
		boost::python::object main;
		boost::python::object script;
		boost::python::object global;
		boost::python::object local;
		boost::python::object result;
		
		std::atomic<bool> _init;

	protected:
		PyContextData(IOData::_Protection priv);

		PyContextData(IODataType dataType);

	public:

		/**
		 * \brief Default empty constructor
		 */
		PyContextData();

		PyContextData(const PyContextData& ref);

		PyContextData(const Data& ref);

		virtual ~PyContextData();


		/**
		 * \brief Copy the image data of the PyContextData object to another one.
		 * \param left The object where to copy the data to
		 */
		virtual void copyTo(PyContextData& left) const;

		PyContextData& operator=(const PyContextData& left);

		//PyContextData& operator=(Data& left);
	};
}
