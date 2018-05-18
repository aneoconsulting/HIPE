//@HIPE_LICENSE@
#pragma once
#include <map>
#include <string>

#include <core/core_export.h>

namespace core
{
	

	class CORE_EXPORT LocalEnv
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

	LocalEnv & getLocalEnv();
}
