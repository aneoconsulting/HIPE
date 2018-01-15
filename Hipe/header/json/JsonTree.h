#pragma once

#include <json/json_export.h>
#include <map>
#include <memory>
#include <core/HipeException.h>
#include <string>

#include <boost/property_tree/ptree_fwd.hpp>




namespace json
{
	class JSON_EXPORT JsonTree;
	
	//typedef boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char>>::iterator ptreeiterator;
	class JSON_EXPORT JsonTree
	{
		std::shared_ptr<boost::property_tree::ptree> _jsonPtree;

		
	public:
		/*class iterator
		{
			JsonTree & tree;
			
			int cur;
			int end;

		public:
			std::string first;
			JsonTree & second;

		public:

			bool operator!=(const iterator & right) const;
			JsonTree::iterator & JsonTree::iterator::operator++();

			iterator(JsonTree & _tree);
			iterator(JsonTree & _tree, int cur);
		};*/



	public:

		JsonTree();

		JsonTree(boost::property_tree::ptree & _js);

		virtual ~JsonTree();

		JsonTree & Add(std::string key, std::string value) ;

		JsonTree & AddInt(std::string key, int value) ;

		JsonTree & AddChild(std::string key, JsonTree & value) ;

		size_t count(std::string) const;

		void set_json_tree(boost::property_tree::ptree ptree);

		boost::property_tree::ptree & get_child(const char* str) const;

	/*	std::shared_ptr<JsonTree::iterator> begin();

		std::shared_ptr<JsonTree::iterator> end();
*/
		std::map<std::string, JsonTree> allchildren(const char* name)const;
		std::shared_ptr<boost::property_tree::ptree> getPtree() const;

		template <typename Type>
			Type get(std::string path) const
		{
			/*return _jsonPtree->get<Type>(path);*/
			throw HipeException("Type isn't ready to use in Hipe");
		}
		
		std::string getString(const std::string& path) const;

		template <typename T>
			T get_value()
		{
			throw HipeException("Type isn't ready to use in Hipe");
		}
		
		double getDouble(const std::string& cs) const;

		float getFloat(const std::string& path) const;

		bool getBool(std::string path) const;
		
		std::string get(std::string path) const;

		
		
		JsonTree& put(std::string key, std::string value);

		void push_back(std::string p1, JsonTree& p2);

		

		bool getInt(std::string path) const;
		
		void read_json(std::istream& stream);

		void write_json(std::ostream& data_response) const;
		
	};


	template <> int JsonTree::get_value();
	template <> std::string JsonTree::get_value();
	template <> bool JsonTree::get_value();
	template <> float JsonTree::get_value();
	template <> double JsonTree::get_value();

	template <> int JsonTree::get<int>(std::string path) const;
	template <> std::string JsonTree::get<std::string>(std::string path) const;
	template <> bool JsonTree::get<bool>(std::string path) const;
	template <> float JsonTree::get<float>(std::string path) const;
	template <> double JsonTree::get<double>(std::string path) const;

	
	JSON_EXTERN template JSON_EXPORT int JsonTree::get_value<int>();
	JSON_EXTERN template JSON_EXPORT std::string JsonTree::get_value<std::string>();
	JSON_EXTERN template JSON_EXPORT bool JsonTree::get_value<bool>();
	JSON_EXTERN template JSON_EXPORT double JsonTree::get_value<double>();
	JSON_EXTERN template JSON_EXPORT float JsonTree::get_value<float>();
	
	JSON_EXTERN template JSON_EXPORT int JsonTree::get<int>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT std::string JsonTree::get<std::string>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT bool JsonTree::get<bool>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT double JsonTree::get<double>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT float JsonTree::get<float>(std::string path) const;





}
