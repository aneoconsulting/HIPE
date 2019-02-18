//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */


#include <json/JsonTree.h>

#pragma warning(push, 0)
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#pragma warning(pop)

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

	int JsonTree::getInt(std::string path) const
	{
		return _jsonPtree->get<int>(path);
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

	template <> char JsonTree::get_value()
	{
		return _jsonPtree->get_value<char>();
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
	float JsonTree::get<float>(std::string path) const
	{
		return getFloat(path);
	}

	template <>
	double JsonTree::get<double>(std::string path) const
	{
		return getDouble(path);
	}

	template <>
	char JsonTree::get<char>(std::string path) const
	{
		return _jsonPtree->get<char>(path);
	}
	

	template <>	void JsonTree::put<int>(std::string key, int value)
	{
		_jsonPtree->put(key, value);

		
	}
	
	template <>	void JsonTree::put<std::string>(std::string key, std::string value)
	{
		_jsonPtree->put(key, value);
		
	}

	template <>	void JsonTree::put<bool>(std::string key, bool value)
	{
		_jsonPtree->put(key, value);
		
	}

	template <>	void JsonTree::put<float>(std::string key, float value)
	{
		_jsonPtree->put(key, value);
		
	}

	template <>	void JsonTree::put<double>(std::string key, double value)
	{
		_jsonPtree->put(key, value);
		
	}

	template <>	void JsonTree::put<char>(std::string key, char value)
	{
		_jsonPtree->put(key, value);

	}
}
