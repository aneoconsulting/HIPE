#pragma once

#include <map>
#include <memory>
#include <filter/Model.h>
#include <core/HipeStatus.h>

namespace filter {
	
	/**
	 * \brief The IFilter interface is used to specialize a filter to process image based data.
	 * \todo
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

		/**
		 * \brief Get the RootFilter node (head) of the current graph
		 * \return A pointer to the RootFilter node of the graph
		 */
		virtual IFilter * getRootFilter();

		virtual void getParent();

		virtual void getNextChildren();

		/**
		 * \brief Get the result of the processing done by the node. This method will return information only. The actual processing is done by the process() method
		 * \return A string containing the result of the processing done by the filter.
		 */
		virtual std::string resultAsString() { return std::string("TODO"); };

		/**
		 * \brief Create a parent child dependency between two filters. Every parent will be processed before its child.
		 * \param filter The filter to add as dependency
		 */
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
