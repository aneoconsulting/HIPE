#pragma once
#include <string>
#include <filter/tools/StringTools.h>
#include <opencv2/core/traits.hpp>
#include <core/HipeException.h>


namespace filter
{
	namespace data
	{
		// only IF(Image from file) has been implmented for now !!
		//The hexavalue is a string representation for serialization
		enum IODataType : long
		{
			SIMG = 0x53494d47, //Simple Image from base64 data
			IMGF = 0x494d4746, // Image from File
			SEQIMG = 0x534551494d47, // Sequence of Image from base64 data source
			SEQIMGD = 0x534551494d4744, // Sequence of image from directory
			SEQVID = 0x534551564944, //Sequence of video from source (Not sure to implement this solution later
			SEQVIDD = 53455156494444, //Sequence of video from directory
			STRMVID = 0x5354524d564944, //Streaming Video from capture device 
			STRMVIDF = 0x5354524d56494446, // Streaming video from file 
			NONE
		};

		class DataTypeMapper
		{
		public:
			static IODataType getTypeFromString(const std::string dataTypeString)
			{
				//convert string to hexa long value;
				long value = ToHex(dataTypeString);
				IODataType dataType = static_cast<IODataType>(value);
				if (value < 0L || dataType < 0L)
				{
					throw HipeException(dataTypeString + " : Bad data type. Cannot find the corresponding type");
				}
				return dataType;
			}
		};
	}
}
