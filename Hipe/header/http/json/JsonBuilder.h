#pragma once
#include <filter/IFilter.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <filter/tools/RegisterTable.h>
#include <Response.h>
#include <HttpServer.h>
#include <json/JsonFilterNode/JsonFilterNode.h>
#include <json/JsonFilterNode/JsonFilterTree.h>
#include <core/HipeException.h>

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
				JsonFilterTree tree;

				for (auto & filter : filters)
				{
					if (filter.second.count("filter") == 0)
						throw HipeException("Cannot find filter");
					boost::property_tree::ptree child = filter.second.get_child("filter");


					std::string type = child.get<std::string>("type");
					std::string name = child.get<std::string>("name");

					

					
					res = newFilter(type);
					res->setname(name);


					JsonFilterNode json_filter_node = JsonFilterNode(res, child);
					tree.add(json_filter_node);

					
					
					//TESTER
					dataResponse += child.get<std::string>("type");
					dataResponse += " ";
					dataResponse += child.get<std::string>("name");

					dataResponse += "; ";
				}
				tree.freeze();
				return res;
			}

		};
	}
}
