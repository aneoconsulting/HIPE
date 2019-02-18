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

#include <corefilter/IFilter.h>

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
