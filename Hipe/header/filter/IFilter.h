#pragma once

#include <string>
#include <vector>
#include <map>
#include <core/HipeException.h>

namespace filter {
	class IFilter
	{
	protected:
		std::string _constructor; 
		std::map<std::string, IFilter *> _parentFilters;
		std::map<std::string, IFilter *> _childFilters;
		std::string _name;
		int _level;
	public:
		IFilter() : _level(0)
		{
			
		}
		IFilter(std::string contructorName) : _constructor(contructorName), _level(0)
		{

		}
		
	public:
		virtual void getNextFilter();

		virtual void getPreviousFilter();

		virtual IFilter * getRootFilter();

		virtual void getParent();

		virtual void getNextChildren();

		virtual std::string resultAsString() = 0;

		void setLevel(int level) { _level = level; }
		int getLevel() { return _level; }
		void setName(std::string name) { _name = name; }
		const std::string & getName() const { return _name; }
		
		const std::string & getConstructorName() const { return _constructor; }

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
