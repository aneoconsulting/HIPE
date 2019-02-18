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
#include <coredata/Data.h>

#include <coredata/data_export.h>


namespace data
{
	/**
	 * \todo
	 * \brief [TODO]
	 * \tparam Base
	 * \tparam Derived
	 */
	class Data;
	template <typename Base, typename Derived>	class IOData;

	template <typename Base, typename Derived>
	class DATA_EXPORT IOData : public Base
	{
	public:
		using Base::Base;

	protected:

		class _Protection
		{
		public:
			_Protection() {}
		};

	public:
		IOData() : Base()
		{

		}

		IOData(const Base& base) : Base(base)
		{

		}


		virtual ~IOData()
		{
			release();
		}

		void release()
		{
			Base::release();
		}

		
		/**
		 * \brief Depracated to review  no way to copy on left if it's const ....
		 * \param left
		 */
		virtual void copyTo(IOData& left) const
		{
			static_cast<const Derived&>(*(Base::_This)).copyTo(static_cast<Derived&>(left));
		}

		virtual bool empty() const
		{
			const Derived& down = static_cast<const Derived&>(This_const());
			return down.empty();

		}

		inline const Derived &  This_const() const
		{
			if (this->_decorate == false) return static_cast<const Derived &>(*(this));

			if (!Base::_This) throw HipeException("Undecorated instance is empty");

			return static_cast<const Derived &>(*((Base::_This).get()));
		}

		inline Derived &  This()
		{
			if (this->_decorate == false) return static_cast<Derived &>(*(this));

			if (!Base::_This) throw HipeException("Undecorated instance is empty");

			return static_cast<Derived &>(*((Base::_This).get()));
		}

		/*inline Derived & operator=(const Data& left)
		{
			Data::registerInstance(left);
			Data::_type = left.getType();
			Data::_decorate = left.getDecorate();

			return This();
		}*/

		/*	IOData& operator<<(const IOData& left)
			{
				if (_type != left._type)
					throw HipeException("Cannot add data because types are different");

				throw HipeException("Not yet impelmented check if the  pointer to return is this or left");

				return *this;
			}*/


	};
}
