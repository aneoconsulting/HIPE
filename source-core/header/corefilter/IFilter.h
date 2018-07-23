//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once

#include <map>
#include <memory>
#include <corefilter/Model.h>
#include <corefilter/filter_export.h>
#include <core/HipeStatus.h>
#include <json/JsonTree.h>

namespace filter {
	
	/**
	 * \brief The IFilter interface is used to specialize a filter to process image based data.
	 * \todo
	 */
	class FILTER_EXPORT IFilter : public filter::Model
	{

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

		std::map<std::string, Model *>  & getParents() override
		{ return _parentFilters; }

		std::map<std::string, Model *> & getChildrens() override
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
