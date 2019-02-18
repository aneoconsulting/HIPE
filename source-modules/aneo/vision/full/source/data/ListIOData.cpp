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

#include <data/ListIOData.h>

namespace data
{
	std::vector<Data> ListIOData::getListIoData()
	{
		return This()._listIoData;
	}

	void ListIOData::copyTo(ListIOData& left) const
	{
		left.This()._listIoData.clear();

		left.Add(*this, true);
	}

	void ListIOData::Add(const ListIOData& left, bool copy)
	{
		for (Data data : left._listIoData)
		{
			Data cur_data;
			data.copyTo(cur_data);

			This()._listIoData.push_back(cur_data);
		}
	}

	ListIOData& ListIOData::operator=(const ListIOData& left)
	{
		if (this == &left) return *this;
		if (!_This) Data::registerInstance(new ListIOData());

		This()._type = left._type;
		This()._listIoData.clear();

		for (auto& iodata : left._listIoData)
		{
			This()._listIoData.push_back(iodata);
		}
		return *this;
	}

	IOData<Data, ListIOData>& ListIOData::operator<<(const ListIOData& left)
	{
		if (_type != left._type)
			throw HipeException("Cannot add data because types are different");

		for (auto& data : left._listIoData)
		{
			This()._listIoData.push_back(data);
		}

		return *this;
	}

	bool ListIOData::empty() const
	{
		return This_const()._listIoData.empty();
	}

	const std::vector<Data>& ListIOData::getListData() const
	{
		return This_const()._listIoData;
	}

	void ListIOData::setListData(const std::vector<Data>& left)
	{
		This()._listIoData = left;
	}
}
