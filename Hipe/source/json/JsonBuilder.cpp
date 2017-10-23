#include <JsonBuilder.h>

namespace json
{

	json::JsonFilterTree * JsonBuilder::buildAlgorithm(std::stringstream& dataResponse, json::JsonTree & treeRequest)
	{
		std::string OK = "Request OK";

		if (treeRequest.count("name") == 0)
			throw HipeException("The algorithm name is not found in the Json. Please inform the field \"name\" : \"name of algorithm\"");

		std::string algoName = treeRequest.get("name");



		json::JsonTree filters = treeRequest.get_child("filters");

		json::JsonFilterTree * tree = new JsonFilterTree();;
		tree->setName(algoName);

		for (auto filter = filters.begin(); filter != filters.end(); ++filter)
		{
			auto element = filter->second.begin(); //first and unique element of filter
			auto child = new JsonTree(element->second);
			auto type = element->first;
			auto name = child->get("name");
			auto res = static_cast<filter::Model *>(newFilter(type));
			res->setName(name);
			auto json_filter_node = JsonFilterNode(res, *child);
			json_filter_node.applyClassParameter();

			tree->add(json_filter_node);


			//TESTER
			dataResponse << type;
			dataResponse << " ";
			dataResponse << child->get("name");

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
		
		return tree;
	}
}
