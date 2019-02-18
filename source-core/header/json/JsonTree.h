//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once

#include <json/json_export.h>
#include <map>
#include <memory>
#include <core/HipeException.h>
#include <string>

#pragma warning(push, 0)
#include <boost/property_tree/ptree_fwd.hpp>
#pragma warning(pop)



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

		JsonTree & add_child(std::string key, JsonTree & value) ;

		size_t count(std::string) const;

		void set_json_tree(boost::property_tree::ptree ptree);

		boost::property_tree::ptree & get_child(const char* str) const;
		bool empty()const;
		JsonTree& add_child_to_child(std::string key, std::string key2, JsonTree & value);

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

		template <typename T>
		void put(std::string key, T value)
		{
			/*return _jsonPtree->get<Type>(path);*/
			throw HipeException("Type isn't ready to use in Hipe");
		}

		void push_back(std::string p1, JsonTree& p2);

		

		int getInt(std::string path) const;
		
		void read_json(std::istream& stream);

		void write_json(std::ostream& data_response) const;
		
	};


	template <> int JsonTree::get_value();
	template <> std::string JsonTree::get_value();
	template <> bool JsonTree::get_value();
	template <> float JsonTree::get_value();
	template <> double JsonTree::get_value();
	template <> char JsonTree::get_value();

	template <> int JsonTree::get<int>(std::string path) const;
	template <> std::string JsonTree::get<std::string>(std::string path) const;
	template <> bool JsonTree::get<bool>(std::string path) const;
	template <> float JsonTree::get<float>(std::string path) const;
	template <> double JsonTree::get<double>(std::string path) const;
	template <> char JsonTree::get<char>(std::string path) const;

	template <>	void JsonTree::put<int>(std::string key, int value);
	template <>	void JsonTree::put<std::string>(std::string key, std::string value);
	template <>	void JsonTree::put<bool>(std::string key, bool value);
	template <>	void JsonTree::put<float>(std::string key, float value);
	template <>	void JsonTree::put<double>(std::string key, double value);
	template <>	void JsonTree::put<char>(std::string key, char value);
	
	JSON_EXTERN template JSON_EXPORT int JsonTree::get_value<int>();
	JSON_EXTERN template JSON_EXPORT std::string JsonTree::get_value<std::string>();
	JSON_EXTERN template JSON_EXPORT bool JsonTree::get_value<bool>();
	JSON_EXTERN template JSON_EXPORT double JsonTree::get_value<double>();
	JSON_EXTERN template JSON_EXPORT float JsonTree::get_value<float>();
	JSON_EXTERN template JSON_EXPORT char JsonTree::get_value<char>();
	
	JSON_EXTERN template JSON_EXPORT int JsonTree::get<int>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT std::string JsonTree::get<std::string>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT bool JsonTree::get<bool>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT float JsonTree::get<float>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT double JsonTree::get<double>(std::string path) const;
	JSON_EXTERN template JSON_EXPORT char JsonTree::get<char>(std::string path) const;

	JSON_EXTERN template JSON_EXPORT void JsonTree::put<int>(std::string key, int value);
	JSON_EXTERN template JSON_EXPORT void JsonTree::put<std::string>(std::string key, std::string value);
	JSON_EXTERN template JSON_EXPORT void JsonTree::put<bool>(std::string key, bool value);
	JSON_EXTERN template JSON_EXPORT void JsonTree::put<float>(std::string key, float value);
	JSON_EXTERN template JSON_EXPORT void JsonTree::put<double>(std::string key, double value);
	JSON_EXTERN template JSON_EXPORT void JsonTree::put<char>(std::string key, char value);

}
