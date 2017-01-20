#include <filter/IFilter.h>

using namespace filter;


void IFilter::getNextFilter()
{
	// TODO - implement IFilter::getNextFilter
	throw "Not yet implemented";
}

void filter::IFilter::getPreviousFilter()
{
	// TODO - implement IFilter::getPreviousFilter
	throw "Not yet implemented";
}

filter::IFilter * filter::IFilter::getRootFilter()
{
	if (_parentFilters.empty()) return this;

	return getRootFilter();
}

void filter::IFilter::getParent()
{
	// TODO - implement IFilter::getParent
	throw "Not yet implemented Multiple Paretns !!";
}

void filter::IFilter::getNextChildren()
{
	// TODO - implement IFilter::getNextChildren
	throw "Not yet implemented Need Iteraror as state and I don't like it";
}

void filter::IFilter::addDependencies(IFilter* child)
{
	if (child->_parentFilters.find(this->_name) != child->_parentFilters.end())
	{
		std::string errorMessage = std::string("Filter named ");
		errorMessage += this->_name;
		errorMessage += " doesn't exist";

		throw std::exception(errorMessage.c_str());
	}
	child->_parentFilters[this->_name] = this;
}

void filter::IFilter::addChildDependencies(IFilter* child)
{
	if (_childFilters.find(this->_name) != _childFilters.end())
	{
		std::string errorMessage = std::string("Filter named ");
		errorMessage += child->_name;
		errorMessage += " doesn't exist";

		throw std::exception(errorMessage.c_str());
	}
	this->_childFilters[child->_name] = (child);
}
