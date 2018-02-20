#pragma once
#include <string>
#include <core/StringTools.h>
#include <core/HipeException.h>
#include <algorithm>

#include <data/data_export.h>

namespace data
{
	// only IF(Image from file) has been implmented for now !!
	//The hexavalue is a string representation for serialization
	/**
	* \brief The multiple data types already implemented
	*/
	enum IODataType : hUInt64
	{
		ACK = 0x41434b,					// This is an acknowledge packet
		SIMG = 0x53494d47,				// Simple Image from base64 data
		IMGF = 0x494d4746,				// Image from File
		LISTIO = 0x4c495354494f,		// List of multiple data types
		SEQIMG = 0x534551494d47,		// Sequence of Image from base64 data source
		SEQIMGD = 0x534551494d4744,		// Sequence of image from directory
		VIDF = 0x56494446,				// Video from file
		SEQVID = 0x534551564944,		// Sequence of video from source (Not sure to implement this solution later
		SEQVIDD = 53455156494444,		// Sequence of video from directory
		STRMVID = 0x5354524d564944,		// Streaming Video from capture device 
		STRMVIDF = 0x5354524d56494446,	// Streaming video from file

		PATTERN = 0x5041545445524e,		// Pattern type with crop and data source
		DIRPATTERN = 0x444952504154,		// DIRPattern type with crop and data source
		SQR_CROP = 0x5351525f43524f50,	// Square crop

		IMGB64 = 0x494d47423634,		// Image from base64 data
		IMGENC = 0x494d474544415441,	// Image encoded (jpg, png...) data

		TXT = 0x545854,
		TXT_ARR = 0x5458545f415252,

		SHAPE = 0x5348415045,			// Shape data (circle, rectangle or point)
		MATCHER = 0x4d415443484552,
		DLIBDTCT = 0x444c494244544354,	// dlib detectors data type (used to shared training data between filters)
		PYCTX = 0x5059435458,
		NONE
	};

	class DATA_EXPORT DataTypeMapper
	{
	public:
		/**
		* \brief Returns the associated \see IODataType enum value from its corresponding name in text
		* \param dataTypeString The name of the requested data type
		* \return The data type as an \see IODataType enum value
		*/
		static IODataType getTypeFromString(const std::string dataTypeString);

		/**
		* \brief
		* \param dataType An \see IODataType enum value
		* \return The name as an std::string object associated with \see IODataType enum value
		*/
		static std::string getStringFromType(const IODataType& dataType);

		/**
		* \brief Checks if a data type is a streamed type one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a streaming type one
		*/
		static bool isStreaming(const IODataType& dataType);

		/**
		* \brief Checks if a data type is an image type one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is an image type one
		*/
		static bool isImage(const IODataType& dataType);

		/**
		* \brief Checks if a data type is a video type one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a video type one
		*/
		static bool isVideo(const IODataType& dataType);

		/**
		* \brief Checks if a data type is a sequence type one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a sequence type one
		*/
		static bool isSequence(const IODataType& dataType);


		static bool isSequenceDirectory(const IODataType & dataType)
		{
			/*if (dataType == IODataType::LISTIO)
			return true;*/
			std::string typeStr = getStringFromType(dataType);

			std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

			if (typeStr.find("seqimgd") != std::string::npos)
				return true;

			return false;
		}
		/**
		* \brief Checks if a data type is a list type one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a list type one
		*/
		static bool isListIo(const IODataType& dataType);

		/**
		* \brief Checks if a data type is a pattern type one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a pattern type one
		*/
		static bool isPattern(const IODataType& dataType);

		/**
		* \brief Checks if a data type is a base64 one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a base64 one
		*/
		static bool isBase64(const IODataType& dataType);

		/**
		* \brief Checks if a data type is a points one
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a points one
		*/
		static bool isShape(const IODataType & dataType);

		/**
		* \brief Checks if a data type is a NoneData
		* \param dataType The queried data type
		* \return Returns true if the queried data type is a points one
		*/
		static bool isNoneData(IODataType dataType);
	};
}
