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
#include <corefilter/Model.h>
#include <corefilter/tools/JsonFilterNode/JsonFilterNode.h>
#include <core/HipeException.h>
#include <corefilter/filter_export.h>

namespace json
{
	class FILTER_EXPORT JsonFilterTree : public filter::Model
	{
		std::map<std::string, Model *> _filterMap;

		//Check if the fully nodes are loaded;
		//If not no dependencies node computation can be accomplish
		bool isFreezed;

	public:
		JsonFilterTree();

		JsonFilterTree(const JsonFilterTree& jTree);


		void freeze();

		///
		/// Add Filter node and insert name of the dependent's parent inside the filter
		/// 
		void add(JsonFilterNode& filterNode);

		void plugPerfAnalysis(Model* nodeToAnalysis, Model* startPerfTime);

		//Link every filters between them
		void computeLinkDependencies();

		//Compute the layer inder of the node when it will be executed
		void computeLevelNode();

		//Find the root
		filter::Model* getRootNode();

		virtual void addDependencies(Model* filter) override;

		virtual void addChildDependencies(Model* filter) override;

		virtual void addDependenciesName(std::string filter) override;

		virtual void addChildDependenciesName(std::string filter) override;

		bool perftimerBuilder;
		
		bool needPerfTimeBuilder() const { return perftimerBuilder;  }

		void disablePerfTimerBuilder()
		{
			perftimerBuilder = false;
		};

		std::map<std::string, Model*> empty;

		virtual std::map<std::string, Model*>& getParents() override;

		virtual std::map<std::string, Model*>& getChildrens() override;


		HipeStatus process() override;

		Model& operator<<(data::Data& element) override;

		Model& operator<<(cv::Mat& element) override;

		virtual data::ConnexDataBase& getConnector() override;
	};
}
