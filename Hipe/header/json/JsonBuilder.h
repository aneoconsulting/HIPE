#pragma once
#include <filter/IFilter.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <filter/tools/RegisterTable.h>
#include <json/JsonFilterNode/JsonFilterNode.h>
#include <json/JsonFilterNode/JsonFilterTree.h>
#include <core/HipeException.h>

namespace json
{
	class JsonBuilder
	{
	public:

		static json::JsonFilterTree * buildAlgorithm(std::stringstream& dataResponse, boost::property_tree::ptree & treeRequest);

		static std::string getOrBuildOrchestrator(std::stringstream& data_response, boost::property_tree::ptree& treeRequest)
		{
			std::string orchestrator;

			if (treeRequest.count("orchestrator") == 0)
				orchestrator = "DefaultScheduler";
			else
			{
				orchestrator = treeRequest.get<std::string>("orchestrator");
			}

			if (orchestrator.empty())
			{
				boost::property_tree::ptree & orchestratorNode = treeRequest.get_child("orchestrator");
				//orchestrator::OrchestratorFactory::getInstance()->addModel(json_filter_tree->getName(), json_filter_tree, orchestrator);
				throw HipeException("Do we really want to deserialize an orchestrator object ?");

			}
			
			data_response << "The orchestrator will " << orchestrator << std::endl;

			return orchestrator;
		}
	};
}

