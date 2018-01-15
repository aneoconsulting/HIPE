#pragma once

#include <json/json_export.h>
#include <map>
#include <memory>
#include <json/PtreeIterator.h>
#include <core/HipeException.h>
#include <string>

//#include <boost/property_tree/ptree_fwd.hpp>


namespace boost
{
	namespace property_tree
	{
		template < class Key, class Data, class KeyCompare = std::less<Key> >
		class basic_ptree;
		typedef basic_ptree<std::string, std::string> ptree;
	}
}




namespace json
{
	

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

		boost::property_tree::ptree get_child(const char* str) const;

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

		/*template <> std::string get(std::string path) const
		{
			return getString(path);
		}
*/
		double getDouble(const std::string& cs) const;

		/*template <> double get(std::string path) const
		{
			return getDouble(path);
		}*/

		
		float getFloat(const std::string& path) const;


		/*template <> float get(std::string path) const
		{
			return getFloat(path);
		}*/

		bool getBool(std::string path) const;
		/*template <> bool get(std::string path) const
		{
			return getBool(path);
		}*/
		
		std::string get(std::string path) const;

		
		
		JsonTree& put(std::string key, std::string value);

		void push_back(std::string p1, JsonTree& p2);

		

		bool getInt(std::string path) const;
		
		void read_json(std::istream& stream);

		void write_json(std::ostream& data_response) const;
		
	};

	JSON_EXTERN template JSON_EXPORT int JsonTree::get<int>(std::string path) const;



}
