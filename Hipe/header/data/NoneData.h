#pragma once

#include <data/IODataType.h>
#include <string>


#include <data/data_export.h>
#include <data/IOData.h>


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
