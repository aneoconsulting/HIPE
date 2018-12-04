//@HIPE_LICENSE@
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
