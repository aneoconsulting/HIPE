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

#include <data/pyObjectData.h>



namespace data
{
	PyObjectData::PyObjectData(IOData::_Protection priv): IOData(PYOBJ)
	{
	}

	PyObjectData::PyObjectData(IODataType dataType): IOData(dataType)
	{
		/*Data::registerInstance(new PyObjectData(IOData::_Protection()));
		This()._type = dataType;
		This()._array.resize(1);*/
	}

	PyObjectData::PyObjectData(): IOData(PYOBJ)
	{
		Data::registerInstance(new PyObjectData(IOData::_Protection()));
		This()._type = PYOBJ;
		
		
		This()._init = false;
	}

	PyObjectData::PyObjectData(const PyObjectData& ref): IOData(PYOBJ)
	{
		Data::registerInstance(ref._This);
		This()._type = ref.This_const()._type;
		_decorate = ref._decorate;
	}

	PyObjectData::PyObjectData(const Data& ref): IOData(PYOBJ)
	{
		if (ref.getType() != IMGF) throw HipeException(
			"ERROR data::PyObjectData::PyObjectData - Only Connexdata should call this constructor.");

		Data::registerInstance(ref);
		This()._type = ref.getType();
		_decorate = ref.getDecorate();
	}

	PyObjectData::~PyObjectData()
	{
		IOData::release();
	}

	void PyObjectData::copyTo(PyObjectData& left) const
	{
		if (getType() != left.getType())
			throw HipeException("Cannot left argument in a PyObjectData");
		

		PyObjectData::copyTo(static_cast<PyObjectData &>(left));
	}

	PyObjectData& PyObjectData::operator=(const PyObjectData& left)
	{
		//if (left.getType() != PYCTX) throw HipeException("ERROR data::PyObjectData::PyObjectData - Only Connexdata should call this constructor.");

		Data::registerInstance(left);
		This()._type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}

	boost::python::object & PyObjectData::get()
	{
		return result;
	}

	void PyObjectData::set(const boost::python::object & _result)
	{
		result = _result;
	}

}
