#pragma once
#include <core/HipeException.h>
#include "JsonFilterNode/JsonFilterTree.h"
#include "http/JsonTree.h"
namespace json
{
	class JsonBuilder
	{
	public:

		static JsonFilterTree * buildAlgorithm(std::stringstream& dataResponse, http::JsonTree & treeRequest);

		static std::string getOrBuildOrchestrator(std::stringstream& data_response, http::JsonTree& treeRequest)
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
	};
}

