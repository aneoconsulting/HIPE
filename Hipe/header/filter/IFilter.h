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
		IFilter() : _level(0)
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
		void setname(std::string name) { _name = name; }
		std::string & getname() { return _name; }

		virtual void addDependencies(IFilter *filter);
		virtual void addChildDependencies(IFilter *filter);

		virtual void addDependenciesName(std::string filter);
		virtual void addChildDependenciesName(std::string filter);

		std::map<std::string, IFilter *>  getParents() const
		{ return _parentFilters; }

		std::map<std::string, IFilter *> getChildrens() const
		{ return _childFilters; }

		
	};
}
