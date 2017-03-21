#pragma once
#include <string>
#include <core/StringTools.h>
#include <core/HipeException.h>


namespace filter
{
	namespace data
	{
		// only IF(Image from file) has been implmented for now !!
		//The hexavalue is a string representation for serialization
		enum IODataType : long
		{
			ACK = 0x41434b, // this is an acknowledge packet
			SIMG = 0x53494d47, //Simple Image from base64 data
			IMGF = 0x494d4746, // Image from File
			SEQIMG = 0x534551494d47, // Sequence of Image from base64 data source
			SEQIMGD = 0x534551494d4744, // Sequence of image from directory
			VIDF = 0x56494446,
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
			static std::string getStringFromType(const IODataType & dataType)
			{
				//convert string to hexa long value;
				std::string value = ToString(dataType);
				
				return value;
			}

			static bool isStreaming(const IODataType & dataType)
			{
				std::string typeStr = getStringFromType(dataType);

				std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

				if (typeStr.find("strm") != std::string::npos)
					return true;

				return false;
			}

			static bool isImage(const IODataType & dataType)
			{
				std::string typeStr = getStringFromType(dataType);

				std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

				if (typeStr.find("img") != std::string::npos)
					return true;

				return false;
			}

			static bool isVideo(const IODataType & dataType)
			{
				std::string typeStr = getStringFromType(dataType);

				std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

				if (typeStr.find("vid") != std::string::npos)
					return true;

				return false;
			}

			static bool isSequence(const IODataType & dataType)
			{
				std::string typeStr = getStringFromType(dataType);

				std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

				if (typeStr.find("seq") != std::string::npos)
					return true;

				return false;
			}
		};
	}
}
