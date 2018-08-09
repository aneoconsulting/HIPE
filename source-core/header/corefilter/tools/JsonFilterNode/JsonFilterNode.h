//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterTable.h>
#include <json/JsonTree.h>
namespace json
{
	class JsonFilterNode
	{
		filter::Model* _filter;
		JsonTree& _params;
	public:
		template <typename T>
		static std::vector<T> as_vector(JsonTree& pt, const char* key);
		std::vector<std::string> getDependenciesFilter() const;

		JsonFilterNode(filter::Model* filter, JsonTree& params) : _filter(filter), _params(params)
		{
		}

		void applyClassParameter() const;
		filter::Model* getFilter() const;
	};
}

