#include <http/JsonTree.h>

	
namespace http
{
	JsonTree::JsonTree()
	{
	}

	JsonTree::~JsonTree()
	{
	}

	JsonTree & JsonTree::Add(std::string key, std::string value)
	{
		jsonPtree.add(key, value);
		return *this;
	}

	JsonTree& JsonTree::AddChild(std::string key, JsonTree& value)
	{
		jsonPtree.add_child(key, value.jsonPtree);
		return *this;
	}

	JsonTree& JsonTree::AddChild(std::string key, boost::property_tree::ptree& value)
	{
		jsonPtree.add_child(key, value);
		return *this;
	}

	void JsonTree::read_json(std::istream stream)
	{
		boost::property_tree::read_json(stream, jsonPtree);
	}

	size_t JsonTree::count(std::string key)
	{
		return jsonPtree.count(key);
	}

	JsonTree &JsonTree::get_child(const char* str)
	{
		auto j = jsonPtree.get_child(str);
		auto returnJ =  new JsonTree(j);
		return *returnJ;
	}

	std::string JsonTree::get(std::string path)
	{
		return jsonPtree.get<std::string>(path);
	}

	boost::property_tree::ptree& JsonTree::get_json_ptree()
	{
		return jsonPtree;
	}


	JsonTree::JsonTree(boost::property_tree::ptree ptree)
	{
		jsonPtree = ptree;
	}
}
