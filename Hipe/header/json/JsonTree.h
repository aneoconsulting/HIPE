#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <json/json_export.h>
#include <map>

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
		size_t count(std::string) const;
		void set_json_tree(boost::property_tree::ptree ptree);
		JsonTree get_child(const char* str)const;
		ptreeiterator begin();
		ptreeiterator end();
		std::map<std::string, JsonTree*> allchildren(char* name)const;
		std::string get(std::string path) const;
		boost::property_tree::ptree &get_json_ptree();
		JsonTree& put(std::string key, std::string value);
		boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>::iterator push_back(std::string p1, JsonTree& p2);
		bool getBool(std::string path) const;
		bool getInt(std::string path) const;
		JsonTree(boost::property_tree::ptree ptree);
		
		void read_json(std::istream& stream);
		void write_json(std::basic_ostream<
			typename boost::property_tree::ptree::key_type::value_type
		>& data_response) const;
	};
}
