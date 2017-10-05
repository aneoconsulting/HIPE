#pragma once
#include <http/AbstractJsonTree.h>
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <json/JsonFilterNode/JsonFilterTree.h>
#include <json/JsonBuilder.h>

namespace http
{
	class JsonTree
	{
		boost::property_tree::ptree jsonPtree;
	public:

		JsonTree();
		virtual ~JsonTree();
		JsonTree & Add(std::string key, std::string value) ;
		JsonTree & AddChild(std::string key, JsonTree & value) ;
		JsonTree & AddChild(std::string key, boost::property_tree::ptree & value) ;
		void read_json(std::istream stream);
		size_t count(std::string);
		JsonTree &get_child(const char* str);
		///Add top layer for ptree method...
		std::string get(std::string path);
		boost::property_tree::ptree &get_json_ptree();
		json::JsonFilterTree *builAlgorithme(std::stringstream &stream);
		std::string getOrBuildOrchestrator(std::stringstream &stream);
	private:
		JsonTree(boost::property_tree::ptree ptree);
	};
}
