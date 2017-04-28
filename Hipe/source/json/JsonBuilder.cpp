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

		for (auto filter = filters.begin(); filter != filters.end(); ++filter)
		{
			auto element = filter->second.begin(); //first and unique element of filter
			boost::property_tree::ptree child = element->second;
			std::string type = element->first;
			std::string name = child.get<std::string>("name");
			filter::IFilter * res = (filter::IFilter *)newFilter(type);
			res->setName(name);
			JsonFilterNode json_filter_node = JsonFilterNode(res, child);
			json_filter_node.applyClassParameter();

			tree->add(json_filter_node);


			//TESTER
			dataResponse << type;
			dataResponse << " ";
			dataResponse << child.get<std::string>("name");

			dataResponse << "; ";

		}
		tree->freeze();
		orchestrator::OrchestratorFactory::getInstance()->addModel(algoName, tree);

		return tree;
	}
}
