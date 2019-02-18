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

#include <coredata/Data.h>

namespace data
{
	std::string Data::getLabel() const
	{
		if (_This)
			return _This->getLabel();

		return _label;
	}

	void Data::setLabel(const std::string& cs)
	{
		if (_This) 
			_This->setLabel(cs);
		_label = cs;
	}

	Data::Data(IODataType datatype): _type(datatype)
	{
		_decorate = false;
	}

	Data::Data(): _type(NONE), _decorate(false), _label("no_label")
	{

	}

	Data::Data(const Data& data): _type(data._type), _This(data._This), _decorate(true)
	{
		if (_This) _This->_decorate = false;
		else _decorate = false;
	}

	Data::~Data()
	{
		release();
	}

	void Data::registerInstance(const Data & childInstance)
	{
		if (childInstance.getDecorate() == false)
			throw HipeException("Cannot accept reference to object as This");
		else
			_This = childInstance._This;

		_decorate = true;
		if (_This) _This->_decorate = false;
		else _decorate = false;
	}

	void Data::registerInstance(Data* childInstance)
	{
		if (childInstance->getDecorate() == true)
			_This = childInstance->_This;
		else
			_This.reset(childInstance);

		_decorate = true;
		if (_This) _This->_decorate = false;
		else _decorate = false;
	}

	void Data::registerInstance(std::shared_ptr<Data> childInstance)
	{
		_This = childInstance;

		_decorate = true;

		if (_This) _This->_decorate = false;
		else _decorate = false;
	}

	IODataType Data::getType() const
	{
		return _type;
	}

	bool Data::getDecorate() const
	{
		return _decorate;
	}

	void Data::copyTypeTo(Data& left)
	{
		if (left._type != _type && left._type != NONE)
			throw HipeException("Cannot copy type [ONE] to type [TWO]");

		left._type = _type; // case where left._type == NONE
	}

	void Data::copyTo(Data& left) const
	{
		_This->copyTo(left);
	}

	bool Data::empty() const
	{
		return _This->empty();
	}

	Data& Data::operator=(const Data& left)
	{
		if (_This) _This.reset();

		_This = left._This;
		_type = left._type;
		_decorate = left._decorate;

		return *this;
	}

	void Data::release()
	{
		if (_This)
		{
			_This.reset();
		}
	}

	void Data::setType(const IODataType io_data_type)
	{
		_type = io_data_type;
	}
}
