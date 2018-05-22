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

#include <coredata/IODataType.h>
#include <memory>

namespace data
{
	class DATA_EXPORT Data
	{
	public:
		typedef Data _classtype;
	protected:
		IODataType _type;
		std::shared_ptr<Data> _This;
		bool _decorate = false;
		std::string _label;

	public:
		std::string getLabel() const;

		void setLabel(const std::string& cs);

	protected:
		Data(IODataType datatype);

	public:
		Data();

		Data(const Data& data);

	public:
		virtual ~Data();

		void registerInstance(const Data & childInstance);

		void registerInstance(Data* childInstance);

		void registerInstance(std::shared_ptr<Data> childInstance);


		IODataType getType() const;

		bool getDecorate() const;

		void copyTypeTo(Data& left);

		virtual void copyTo(Data& left) const;

		virtual bool empty() const;

		Data& operator=(const Data& left);

		void release();

		void setType(const IODataType io_data_type);
	};
}
