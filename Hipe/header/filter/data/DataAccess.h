#pragma once 


enum DataAccess : long
{
	COPY = 0x434f5059,
	SHARE = 0x5348415245,
	REF = 0x524546,
	NONE = 0x4e4f4e45
};

class DataAccessMapper
{
public:
	static DataAccess getAccessFromString(const std::string dataAccessString)
	{
		//convert string to hexa long value;
		long value = ToHex(dataAccessString);
		DataAccess data_access = static_cast<DataAccess>(value);
		if (value < 0L || data_access < 0L)
		{
			throw HipeException(dataAccessString + " : Bad data Acess. Cannot find the corresponding type");
		}
		return data_access;
	}
};