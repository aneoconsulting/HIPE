#pragma once

#include <map>
#include <memory>
#include <filter/Model.h>
#include <core/HipeStatus.h>

namespace filter {
	
	/**
	 * \brief The IFilter interface is used to specialize a filter to process image based data.
	 * [TODO]
	 */
	class IFilter : public filter::Model
	{
	protected:
		
		std::map<std::string, Model *> _parentFilters;
		std::map<std::string, Model *> _childFilters;
		
		
	public:
		IFilter() 
		{
			_level = 0;
		}
		IFilter(std::string contructorName) 
		{
			_constructor = contructorName;
			_level = 0;

		}
		
	public:
		virtual void getNextFilter();

		virtual void getPreviousFilter();

		virtual IFilter * getRootFilter();

		virtual void getParent();

		virtual void getNextChildren();

		virtual std::string resultAsString() { return std::string("TODO"); };

		virtual void addDependencies(Model *filter);
		virtual void addChildDependencies(Model *filter);

		virtual void addDependenciesName(std::string filter);
		virtual void addChildDependenciesName(std::string filter);

		virtual std::map<std::string, Model *>  getParents() const
		{ return _parentFilters; }

		virtual std::map<std::string, Model *> getChildrens() const
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
