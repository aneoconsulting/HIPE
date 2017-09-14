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

void filter::IFilter::addDependencies(Model* _parent)
{
	IFilter *parent = static_cast<IFilter *>(_parent);
	//TODO : somewhere else the contract to do not have multiple object with same name (after Json to object is a good place to do it)

	if (!(parent->_childFilters.find(this->_name) != parent->_childFilters.end() && (parent->_childFilters[this->_name] != nullptr)))
	{
		parent->_childFilters[this->_name] = this;
		*(parent) << *(this);
	}

	if (!(this->_parentFilters.find(parent->_name) != this->_parentFilters.end() && (this->_parentFilters[parent->_name] != nullptr)))
	{
		this->_parentFilters[parent->getName()] = parent;
	}
}

void filter::IFilter::addChildDependencies(Model* _child)
{
	IFilter* child = static_cast<IFilter *>(_child);

	if (_childFilters.find(child->_name) != _childFilters.end() && (_childFilters[child->_name] != nullptr))
	{
		std::string errorMessage = std::string("Filter named ");
		errorMessage += child->_name;
		errorMessage += " already exist";

		throw HipeException(errorMessage.c_str());
	}
	this->_childFilters[child->_name] = (child);

	*(this) << *(child);

	if (child->_parentFilters.find(this->_name) != child->_parentFilters.end() && (child->_parentFilters[this->_name] != nullptr))
	{
		std::string errorMessage = std::string("Filter named ");
		errorMessage += this->_name;
		errorMessage += " already exist";

		throw HipeException(errorMessage.c_str());
	}
	child->_parentFilters[this->_name] = this;
}


void filter::IFilter::addDependenciesName(std::string filterName)
{
	_parentFilters[filterName] = nullptr;
}

void filter::IFilter::addChildDependenciesName(std::string filterName)
{
	_childFilters[filterName] = nullptr;
}
