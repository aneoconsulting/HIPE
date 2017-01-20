#pragma once
#include <string>
#include <map>
#include <filter/IFilter.h>

class JsonFilterTree
{
	std::map<std::string, filter::IFilter *> _filterMap;
	public:
	JsonFilterTree()
	{
		
	}

	void add(std::string filterName, filter::IFilter *filter)
	{
		if (_filterMap.find(filterName) != _filterMap.end())
		{
			std::string errorMessage = std::string("Filter named ");
			errorMessage += filterName;
			errorMessage += " doesn't exist";

			throw std::exception(errorMessage.c_str());
		}
		_filterMap[filterName] = filter;


	}

};
