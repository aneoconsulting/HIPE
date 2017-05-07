#pragma once

#include <map>
#include <memory>
#include <filter/Model.h>
#include <core/HipeStatus.h>
#include "data/OutputData.h"

namespace filter {
	
	class IFilter : public filter::Model
	{
	protected:
		std::string _constructor; 
		std::map<std::string, IFilter *> _parentFilters;
		std::map<std::string, IFilter *> _childFilters;
		
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

		virtual std::string resultAsString() { return std::string("TODO"); };

		void setLevel(int level) { _level = level; }
		int getLevel() { return _level; }
		
		
		const std::string & getConstructorName() const { return _constructor; }

		virtual void addDependencies(IFilter *filter);
		virtual void addChildDependencies(IFilter *filter);

		virtual void addDependenciesName(std::string filter);
		virtual void addChildDependenciesName(std::string filter);

		std::map<std::string, IFilter *>  getParents() const
		{ return _parentFilters; }

		std::map<std::string, IFilter *> getChildrens() const
		{ return _childFilters; }

		

		public:
		virtual IFilter &operator<<(data::Data & element)
		{

			return *this;
		}

		virtual IFilter &operator<<(cv::Mat & element)
		{

			return *this;
		}

		virtual IFilter & getCast() { return *this; }

		virtual IFilter &operator<<(IFilter & other)
		{
			this->getConnector() << other.getConnector();
			return *this;
		}

		inline virtual data::ConnexDataBase & getConnector()
		{
			throw HipeException("Cannot get a connector on abstract class IFilter");
		}
		
		
		

	public:

		virtual HipeStatus process() = 0;
		
	};
}
