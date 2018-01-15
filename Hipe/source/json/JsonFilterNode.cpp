#include <json/JsonFilterNode/JsonFilterNode.h>
#include <json/JsonTree.h>
#include <boost/property_tree/ptree.hpp>

namespace json
{
	template <typename T>
	std::vector<T> JsonFilterNode::as_vector(JsonTree& pt, const char* key)
	{
		std::vector<T> r;
		for (auto& item : pt.get_child(key))
			r.push_back(item.second.get_value<T>());
		return r;
	}

	std::vector<std::string> JsonFilterNode::getDependenciesFilter() const
	{
		if (_params.count("need") == 0)
		{
			return std::vector<std::string>();
		}
		return as_vector<std::string>(_params, "need");
	}

	void JsonFilterNode::applyClassParameter() const
	{
		for (auto& field : getParameterNames(_filter->getConstructorName()))
		{
			std::string copyField(field);

			if (_params.count(field) != 0)
			{
				__invoke(_filter, "set_" + copyField + "_from_json", _params);
			}
		}
	}
	filter::Model* JsonFilterNode::getFilter() const
	{
		return _filter;
	}
}