#include <JsonBuilder.h>
#include <orchestrator/Orchestrator.h>

namespace json
{

	json::JsonFilterTree * JsonBuilder::buildAlgorithm(std::stringstream& dataResponse, boost::property_tree::ptree & treeRequest)
	{
		std::string OK = "Request OK";

		if (treeRequest.count("name") == 0)
			return nullptr;

		std::string algoName = treeRequest.get<std::string>("name");



		if (treeRequest.count("filters") == 0)
			return static_cast<json::JsonFilterTree *>(orchestrator::OrchestratorFactory::getInstance()->getModel(algoName));

		boost::property_tree::ptree filters = treeRequest.get_child("filters");

		//std::string name = pt.get<string>("firstName") + " " + pt.get<std::string>("lastName") + " " + pairs.get<std::string>("name");

		json::JsonFilterTree * tree = new JsonFilterTree();;
		tree->setName(algoName);

		for (auto& filter : filters)
		{
			if (filter.second.count("filter") == 0)
				throw HipeException("Cannot find filter");
			boost::property_tree::ptree child = filter.second.get_child("filter");


			std::string type = child.get<std::string>("type");
			std::string name = child.get<std::string>("name");


			filter::IFilter * res = newFilter(type);
			res->setName(name);


			JsonFilterNode json_filter_node = JsonFilterNode(res, child);
			json_filter_node.applyClassParameter();

			tree->add(json_filter_node);


			//TESTER
			dataResponse << child.get<std::string>("type");
			dataResponse << " ";
			dataResponse << child.get<std::string>("name");

			dataResponse << "; ";

		}
		tree->freeze();
		orchestrator::OrchestratorFactory::getInstance()->addModel(algoName, tree);

		return tree;
	}
}
