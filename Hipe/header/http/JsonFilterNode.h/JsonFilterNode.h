#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <filter/IFilter.h>


class JsonFilterNode
{
	filter::IFilter &_filter;
	boost::property_tree::ptree & _params;
	
public:
	JsonFilterNode(filter::IFilter & filter, boost::property_tree::ptree & params) : _filter(filter), _params(params) { }

	void applyClassParameter()
	{
		
	}

	filter::IFilter & getFilter() { return _filter; }


};
