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
#include <string>
#include <core/HipeStatus.h>
#include <coredata/IOData.h>
#include <coredata/DataAccess.h>
#include <coredata/ConnexData.h>
#include <corefilter/filter_export.h>



namespace cv {
	class Mat;
}

namespace filter
{
	struct _protectPreInit
	{
	};

	/**
	 * \brief The Model class is the foundatation of every Hipe's filter. It's the base class they should implement.
	 * \todo
	 */
	class FILTER_EXPORT Model
	{
	protected:
		std::string _name;
		std::string _constructor;
		int _level;

		std::map<std::string, Model *> _parentFilters;
		std::map<std::string, Model *> _childFilters;


		Model() : _level(0)
		{
			_name = "NO_NAME";
			_constructor = "MODEL_ERROR";
		}

	public:
		virtual ~Model() {}
		

		const std::string & getConstructorName() const { return _constructor; }

		void setLevel(int level) { _level = level; }
		int getLevel() { return _level; }

		virtual void cleanUp()
		{

		}

		virtual void dispose() {}


		const std::string & getName() const { return _name; }

		void setName(const std::string& algoName)
		{
			_name = algoName;
		}

		virtual std::string getNamespace() const
		{
			return "Unkown";
		}


		virtual void addDependencies(Model *filter) = 0;
		virtual void addChildDependencies(Model *filter) = 0;

		virtual void addDependenciesName(std::string filter) = 0;
		virtual void addChildDependenciesName(std::string filter) = 0;

		virtual std::map<std::string, Model *>  & getParents()  = 0;


		virtual std::map<std::string, Model *> & getChildrens() = 0;

		Model * getRootFilter()
		{
			if (_parentFilters.empty()) return this;
			std::map<std::string, Model*>::iterator pair = _parentFilters.begin();
			return pair->second->getRootFilter();
		}
		virtual void onLoad(void* data) {};
		
		virtual void onStart(void* data) {};

		virtual bool isPython() { return false; }

		virtual HipeStatus process() = 0;

		virtual Model &operator<<(data::Data & element) = 0;

		virtual Model &operator<<(cv::Mat & element) = 0;

		virtual data::ConnexDataBase & getConnector() = 0;
	};

}
