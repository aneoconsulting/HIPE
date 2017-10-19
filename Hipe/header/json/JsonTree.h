#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <json/JsonFilterNode/JsonFilterTree.h>
#include <json/json_export.h>
namespace json
{

	typedef boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>::iterator ptreeiterator;
	class JSON_EXPORT JsonTree
	{
		boost::property_tree::ptree jsonPtree;

	public:

		JsonTree();
		virtual ~JsonTree();
		JsonTree & Add(std::string key, std::string value) ;
		JsonTree & AddInt(std::string key, int value) ;
		JsonTree & AddChild(std::string key, JsonTree & value) ;
		JsonTree & AddChild(std::string key, boost::property_tree::ptree & value) ;
		void read_json(std::istream stream);
		size_t count(std::string) const;
		JsonTree &get_child(const char* str);
		ptreeiterator begin();
		ptreeiterator end();
		std::map<std::string, JsonTree*> allchildren(char* name);
		std::string get(std::string path) const;
		boost::property_tree::ptree &get_json_ptree();
		JsonTree& put(std::string key, std::string value);
		boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>::iterator push_back(std::string p1, JsonTree& p2);
		bool getBool(std::string path) const;
		bool getInt(std::string path) const;
	private:
		JsonTree(boost::property_tree::ptree ptree);
	};
}
