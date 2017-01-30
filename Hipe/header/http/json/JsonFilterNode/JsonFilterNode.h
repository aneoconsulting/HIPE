#pragma once
#include <boost/property_tree/ptree.hpp>
#include <filter/IFilter.h>

namespace http
{
	namespace json
	{

		class JsonFilterNode
		{
			filter::IFilter *_filter;

			boost::property_tree::ptree & _params;
		private:
			template <typename T>
			std::vector<T> as_vector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
			{
				std::vector<T> r;
				for (auto& item : pt.get_child(key))
					r.push_back(item.second.get_value<T>());
				return r;
			}

		public:
			std::vector<std::string> getDependenciesFilter()
			{
				if (_params.count("need") == 0)
				{
					return std::vector<std::string>();
				}
				return as_vector<std::string>(_params, "need");
			}


			JsonFilterNode(filter::IFilter * filter, boost::property_tree::ptree & params) : _filter(filter), _params(params) { }

			void applyClassParameter()
			{
				for (auto &field : getParameterNames(_filter->getConstructorName()))
				{
					std::string copyField(field);

					if (_params.count(field) != 0)
					{
						__invoke(_filter, "set_" + copyField + "_from_json", _params);

					}
				}
			}

			filter::IFilter * getFilter() { return _filter; }


		};
	}
}
