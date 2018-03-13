#include <json/JsonTree.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace json
{
	/*bool JsonTree::iterator::operator!=(const iterator& right) const
	{
		if (this->cur != right.cur)
			return true;

		return false;
	}

	JsonTree::iterator& JsonTree::iterator::operator++()
	{
		if (cur < end) cur++;

		int tmp = 0;
		auto iter = tree._jsonPtree->begin();
		while (tmp < cur && tmp < tree._jsonPtree->size())
		{
			++iter;
			tmp++;
		}

		return *this;
	}

	JsonTree::iterator::iterator(JsonTree& _tree) : tree(_tree), cur(0), end(_tree._jsonPtree->size()), second(_tree.begin()->second)
	{
	}

	JsonTree::iterator::iterator(JsonTree& _tree, int _cur) : tree(_tree), second(_tree.begin()->second), cur(_cur), end(_tree._jsonPtree->size())
	{
	}*/

	JsonTree::JsonTree()
	{
		_jsonPtree = std::make_shared<boost::property_tree::ptree>();
	}

	JsonTree::JsonTree(boost::property_tree::ptree& _js)
	{
		_jsonPtree = std::make_shared<boost::property_tree::ptree>(_js);
	}

	JsonTree::~JsonTree()
	{
	}

	JsonTree& JsonTree::Add(std::string key, std::string value)
	{
		_jsonPtree->add(key, value);
		return *this;
	}

	JsonTree& JsonTree::AddInt(std::string key, int value)
	{
		_jsonPtree->add<int>(key, value);
		return *this;
	}

	JsonTree& JsonTree::add_child(std::string key, JsonTree& value)
	{
		_jsonPtree->add_child(key, *value._jsonPtree);
		return *this;
	}

	/*JsonTree& JsonTree::AddChild(std::string key, JsonTree& value)
	{
		jsonPtree->add_child(key, value);
		return *this;
	}*/

	float JsonTree::getFloat(const std::string& path) const
	{
		return _jsonPtree->get<float>(path);
	}

	std::string JsonTree::get(std::string path) const
	{
		return _jsonPtree->get<std::string>(path);
	}


	void JsonTree::read_json(std::istream& stream)
	{
		boost::property_tree::read_json(stream, *_jsonPtree);
	}

	void JsonTree::write_json(std::ostream& data_response) const
	{
		boost::property_tree::write_json(data_response, *_jsonPtree);
	}


	size_t JsonTree::count(std::string key) const
	{
		return _jsonPtree->count(key);
	}

	void JsonTree::set_json_tree(boost::property_tree::ptree ptree)
	{
		_jsonPtree = std::make_shared<boost::property_tree::ptree>(ptree);
	}

	boost::property_tree::ptree & JsonTree::get_child(const char* str) const
	{
		
		return _jsonPtree->get_child(str);
	}
	
	JsonTree & JsonTree::add_child_to_child(std::string key1, std::string key2, JsonTree & value) 
	{
		_jsonPtree->get_child(key1).add_child(key2, *value._jsonPtree);
		return *this;
	}

	bool JsonTree::getBool(std::string path) const
	{
		return _jsonPtree->get<bool>(path);
	}

	bool JsonTree::getInt(std::string path) const
	{
		return _jsonPtree->get<int>(path);
	}


	JsonTree& JsonTree::put(std::string key, std::string value)
	{
		auto p = _jsonPtree->put(key, value);
		return *this;
	}

	//boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>::iterator JsonTree::push_back(std::string p1, JsonTree &p2)
	void JsonTree::push_back(std::string p1, JsonTree& p2)
	{
		auto r = _jsonPtree->push_back(std::make_pair(p1, *p2._jsonPtree));
	}

	std::map<std::string, JsonTree> JsonTree::allchildren(const char* name) const
	{
		std::map<std::basic_string<char>, JsonTree> ret;
		auto child = _jsonPtree->get_child(name);
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			JsonTree r(itarray->second);

			std::basic_string<char> data_type = itarray->first;
			ret[data_type] = r;
		}
		return ret;
	}

	std::shared_ptr<boost::property_tree::ptree> JsonTree::getPtree() const
	{
		return _jsonPtree;
	}

	std::string JsonTree::getString(const std::string& path) const
	{
		return _jsonPtree->get<std::string>(path);
	}

	double JsonTree::getDouble(const std::string& path) const
	{
		return _jsonPtree->get<double>(path);
	}
	bool JsonTree::empty() const {
		return _jsonPtree->empty();
	}

	template <> int JsonTree::get_value()
	{
		return _jsonPtree->get_value<int>();
	}

	template <> std::string JsonTree::get_value()
	{
		return _jsonPtree->get_value<std::string>();
	}

	template <> bool JsonTree::get_value()
	{
		return _jsonPtree->get_value<bool>();
	}

	template <> float JsonTree::get_value()
	{
		return _jsonPtree->get_value<float>();
	}

	template <> double JsonTree::get_value()
	{
		return _jsonPtree->get_value<double>();
	}
	
	template <>
	std::string JsonTree::get(std::string path) const
	{
		  return getString(path);
	}

	template <>
	int JsonTree::get<int>(std::string path) const
	{
		return getInt(path);
	}

	template <>
	bool JsonTree::get<bool>(std::string path) const
	{
		return getBool(path);
	}

	template <>
	double JsonTree::get<double>(std::string path) const
	{
		return getDouble(path);
	}

	template <>
	float JsonTree::get<float>(std::string path) const
	{
		return getFloat(path);
	}


}
