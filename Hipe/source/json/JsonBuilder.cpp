#include <JsonBuilder.h>
#include <orchestrator/Orchestrator.h>

namespace json
{

	json::JsonFilterTree * JsonBuilder::buildAlgorithm(std::stringstream& dataResponse, core::JsonTree & treeRequest)
	{
		std::string OK = "Request OK";

		if (treeRequest.count("name") == 0)
			throw HipeException("The algorithm name is not found in the Json. Please inform the field \"name\" : \"name of algorithm\"");

		std::string algoName = treeRequest.get("name");



		if (treeRequest.count("filters") == 0)
			return static_cast<json::JsonFilterTree *>(orchestrator::OrchestratorFactory::getInstance()->getModel(algoName));

		core::JsonTree filters = treeRequest.get_child("filters");

		json::JsonFilterTree * tree = new JsonFilterTree();;
		tree->setName(algoName);

		for (auto filter = filters.begin(); filter != filters.end(); ++filter)
		{
			auto element = filter->second.begin(); //first and unique element of filter
			boost::property_tree::ptree child = element->second;
			std::string type = element->first;
			std::string name = child.get<std::string>("name");
			filter::Model * res = static_cast<filter::Model *>(newFilter(type));
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
		try
		{
			tree->freeze();
		}
		catch (HipeException & e)
		{
			throw;
		}
		orchestrator::OrchestratorFactory::getInstance()->addModel(algoName, tree);
		if (tree == nullptr)
			throw HipeException("fail to build algorithm");
		return tree;
	}
}
