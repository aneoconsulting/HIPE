#include <json/JsonTree.h>


namespace json
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

	JsonTree& JsonTree::AddInt(std::string key, int value)
	{
		jsonPtree.add<int>(key, value);
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

	void JsonTree::read_json(std::istream &stream)
	{
		boost::property_tree::read_json(stream, jsonPtree);
	}

	void JsonTree::write_json(std::basic_ostream<boost::property_tree::ptree::key_type::value_type>& data_response) const
	{
		boost::property_tree::write_json(data_response, jsonPtree);
	}


	size_t JsonTree::count(std::string key) const
	{
		return jsonPtree.count(key);
	}

	void JsonTree::set_json_tree(boost::property_tree::ptree ptree)
	{
		jsonPtree = ptree;
	}

	JsonTree JsonTree::get_child(const char* str)
	{
		auto j = jsonPtree.get_child(str);
		JsonTree res;
		res.set_json_tree(j);
		return res;
	}
	ptreeiterator  JsonTree::begin()
	{
		return jsonPtree.begin();
	}
	ptreeiterator JsonTree::end()
	{
		return jsonPtree.end();
	}
	std::string JsonTree::get(std::string path) const
	{
		return jsonPtree.get<std::string>(path);
	}

	bool JsonTree::getBool(std::string path) const
	{
		return jsonPtree.get<bool>(path);
	}
	bool JsonTree::getInt(std::string path) const
	{
		return jsonPtree.get<int>(path);
	}
	boost::property_tree::ptree& JsonTree::get_json_ptree()
	{
		return jsonPtree;
	}

	JsonTree& JsonTree::put(std::string key, std::string value)
	{
		auto p = jsonPtree.put(key, value);
		JsonTree j;
		j.set_json_tree(p);
		return j;
	}

	boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>::iterator JsonTree::push_back(std::string p1, JsonTree &p2)
	{
		auto r = jsonPtree.push_back(std::make_pair(p1, p2.get_json_ptree()));
		return r;
	}
	JsonTree::JsonTree(boost::property_tree::ptree ptree)
	{
		jsonPtree = ptree;
	}
	std::map<std::string, JsonTree*> JsonTree::allchildren(char* name)
	{
		std::map<std::basic_string<char>, JsonTree*> ret;
		auto child = jsonPtree.get_child(name);
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			JsonTree *r = new JsonTree(itarray->second);
			std::basic_string<char> data_type = itarray->first;
			ret[data_type] = r;
		}
		return ret;
	}
}
