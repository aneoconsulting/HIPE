//@HIPE_LICENSE@
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
