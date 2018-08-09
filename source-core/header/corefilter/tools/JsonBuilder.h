//@HIPE_LICENSE@
#pragma once
#include <core/HipeException.h>
#include <corefilter/tools/JsonFilterNode/JsonFilterTree.h>
#include <json/JsonTree.h>
#include <coredata/OutputData.h>
#include <corefilter/filter_export.h>

namespace json
{
	class FILTER_EXPORT JsonBuilder
	{
	public:

		static JsonFilterTree * buildAlgorithm(std::stringstream& dataResponse, json::JsonTree & treeRequest);

		static std::string getOrBuildOrchestrator(std::stringstream& data_response, json::JsonTree& treeRequest)
		{
			std::string orchestrator;

			if (treeRequest.count("orchestrator") == 0)
				orchestrator = "DefaultScheduler";
			else
			{
				orchestrator = treeRequest.get("orchestrator");
			}

			if (orchestrator.empty())
			{
				//boost::property_tree::ptree & orchestratorNode = treeRequest.get_child("orchestrator");
				throw HipeException("Do we really want to deserialize an orchestrator object ?");

			}
			
			data_response << "The orchestrator will " << orchestrator << std::endl;

			return orchestrator;
		}

		template <typename DataType>
		static json::JsonTree buildJson(const DataType & data)
		{
		
				throw HipeException("Not yet implemented for type " + data.getType());
	
		}


	};

	template <> json::JsonTree JsonBuilder::buildJson<data::OutputData>(const data::OutputData & data);
	
	FILTER_EXTERN template FILTER_EXPORT json::JsonTree JsonBuilder::buildJson<data::OutputData>(const data::OutputData & data);
}

