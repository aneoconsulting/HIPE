#include <filter/IFilter.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <filter/tools/RegisterTable.h>
#include <Response.h>
#include <HttpServer.h>

namespace http {
	namespace json {
		class JsonBuilder
		{
		public:

			static filter::IFilter * buildAlgorithm(std::string & dataResponse, Content & dataRequest)
			{
				boost::property_tree::ptree pt;
				read_json(dataRequest, pt);
				std::string OK = "Request OK";
				boost::property_tree::ptree filters = pt.get_child("filters");

				//std::string name = pt.get<string>("firstName") + " " + pt.get<std::string>("lastName") + " " + pairs.get<std::string>("name");
				filter::IFilter * res = nullptr;

				for (auto & filter : filters)
				{
					if (filter.second.count("filter") == 0)
						throw std::exception("Cannot find filter");
					boost::property_tree::ptree child = filter.second.get_child("filter");

					std::string type = child.get<std::string>("type");
					res = newFilter(type);




					dataResponse += child.get<std::string>("type");
					dataResponse += " ";
					dataResponse += child.get<std::string>("name");

					dataResponse += "; ";
				}
				return res;
			}

		};
	}
}
