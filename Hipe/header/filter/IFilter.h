#pragma once

#include <string>
#include <vector>
#include <map>

namespace filter {
	class IFilter
	{
		std::map<std::string, IFilter *> _parentFilters;
		std::map<std::string, IFilter *> _childFilters;
		std::string _name;
		int _level;
	public:
		IFilter()
		{
			
		}
		
	public:
		virtual void getNextFilter();

		virtual void getPreviousFilter();

		virtual IFilter * IFilter::getRootFilter();

		virtual void getParent();

		virtual void getNextChildren();

		virtual std::string resultAsString() = 0;

		void setLevel(int level) { _level = level; }
		int getLevel() { return _level; }

		virtual void addDependencies(IFilter *filer);
		virtual void addChildDependencies(IFilter *filer);

	};
}
