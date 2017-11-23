#include <data/IODataType.h>

namespace data
{
	IODataType DataTypeMapper::getTypeFromString(const std::string dataTypeString)
	{
		//convert string to hexa long value;
		hUInt64 value = ToHex(dataTypeString);
		IODataType dataType = static_cast<IODataType>(value);
		if (value < 0L || dataType < 0L)
		{
			throw HipeException(dataTypeString + " : Bad data type. Cannot find the corresponding type");
		}
		return dataType;
	}

	std::string DataTypeMapper::getStringFromType(const IODataType& dataType)
	{
		//convert string to hexa long value;
		std::string value = ToString(dataType);

		return value;
	}

	bool DataTypeMapper::isStreaming(const IODataType& dataType)
	{
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("strm") != std::string::npos)
			return true;

		return false;
	}

	bool DataTypeMapper::isImage(const IODataType& dataType)
	{
		if (dataType == IODataType::LISTIO)
			return true;
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("img") != std::string::npos)
			return true;

		return false;
	}

	bool DataTypeMapper::isVideo(const IODataType& dataType)
	{
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("vid") != std::string::npos && typeStr.find("strm") == std::string::npos)
			return true;

		return false;
	}

	bool DataTypeMapper::isSequence(const IODataType& dataType)
	{
		/*if (dataType == IODataType::LISTIO)
			return true;*/
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("seq") != std::string::npos)
			return true;

		return false;
	}

	bool DataTypeMapper::isListIo(const IODataType& dataType)
	{
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("lis") != std::string::npos)
			return true;

		return false;
	}

	bool DataTypeMapper::isPattern(const IODataType& dataType)
	{
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("pattern") != std::string::npos)
			return true;

		return false;
	}

	bool DataTypeMapper::isBase64(const IODataType& dataType)
	{
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("base64") != std::string::npos)
			return true;

		return false;
	}

	bool DataTypeMapper::isShape(const IODataType& dataType)
	{
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("shape") != std::string::npos)
			return true;

		return false;
	}
}
