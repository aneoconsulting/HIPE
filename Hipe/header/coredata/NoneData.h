#pragma once

#include <coredata/IODataType.h>
#include <coredata/data_export.h>
#include <coredata/IOData.h>

#include <string>

namespace boost {
	namespace filesystem {
		class path;
	}
}


namespace data
{
	/**
	* \brief Simple explicit class to a void data
	*/
	class DATA_EXPORT NoneData : public IOData<Data, NoneData>
	{
		
	public:

		/**
		* \brief Default NoneData constructor for private usage only
		*/
		NoneData();

	};
}
