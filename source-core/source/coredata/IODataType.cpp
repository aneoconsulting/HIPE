//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <coredata/IODataType.h>

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
		if (dataType == IODataType::NONE)
			return "NONE";

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

		if (typeStr.find("pattern") != std::string::npos || typeStr.find("dirpat")!=std::string::npos)
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

	bool DataTypeMapper::isNoneData(IODataType dataType)
	{
		std::string typeStr = getStringFromType(dataType);

		std::transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

		if (typeStr.find("none") != std::string::npos)
			return true;

		return false;
	}
}
