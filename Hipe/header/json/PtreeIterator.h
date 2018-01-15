#pragma once
#include <functional>


namespace boost
{
	namespace property_tree
	{
		template < class Key, class Data, class KeyCompare = std::less<Key> >
		class basic_ptree;

		typedef basic_ptree<std::string, std::string> ptree;
		
	}
}

