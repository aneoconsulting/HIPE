//@HIPE_LICENSE@
#pragma once
#include <map>
#include <string>

#include <corefilter/filter_export.h>

namespace corefilter
{
	

	class FILTER_EXPORT LocalEnv
	{
	private:
		std::map<std::string, std::string> envValues;


	public:
		void setValue(std::string key, std::string value)
		{
			envValues[key] = value;

		}

		std::string getValue(std::string key)
		{
			if (envValues.find(key) == envValues.end()) 
				return std::string("None");

			return envValues[key];
		}
	};

	FILTER_EXPORT LocalEnv & getLocalEnv();
}
