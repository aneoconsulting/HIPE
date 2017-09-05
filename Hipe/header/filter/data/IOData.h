#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include "IODataType.h"
#include <memory>


namespace filter
{
	namespace data
	{
		/**
		 * \brief [TODO]
		 */
		class Data;
		template <typename Base, typename Derived>	class IOData;

		
		class Data
		{
		public:
		public:
			typedef Data _classtype;
		protected:
			IODataType _type;
			std::shared_ptr<Data> _This;
			bool _decorate = false;

		
		public:
			Data() : _type(NONE), _decorate(false)//, This(std::make_shared<Data>(nullptr))
			{
				
			}

			Data(const Data& data) : _type(data._type), _This(data._This), _decorate(true)
			{

				if (_This) _This->_decorate = false;
				else _decorate = false;
			}

		public:
			virtual ~Data()
			{
				release();
			}

			inline void registerInstance(const Data & childInstance)
			{
				if (childInstance.getDecorate() == false)
					throw HipeException("Cannot accept reference to object as This");
				else
					_This = childInstance._This;
				
				_decorate = true;
				if (_This) _This->_decorate = false;
				else _decorate = false;
			}

			inline void registerInstance(Data * childInstance)
			{
				if (childInstance->getDecorate() == true)
					_This = childInstance->_This;
				else
					_This.reset(childInstance);

				_decorate = true;
				if (_This) _This->_decorate = false;
				else _decorate = false;
			}

			inline void registerInstance(std::shared_ptr<Data> childInstance)
			{
				_This = childInstance;
				
				_decorate = true;

				if (_This) _This->_decorate = false;
				else _decorate = false;
			}


			IODataType getType() const
			{
				return _type;
			}
			
			bool getDecorate() const
			{
				return _decorate;
			}

			void copyTypeTo(Data& left)
			{
				if (left._type != _type && left._type != NONE)
					throw HipeException("Cannot copy type [ONE] to type [TWO]");

				left._type = _type; // case where left._type == NONE
			}

			virtual void copyTo(Data& left) const
			{
				
				_This->copyTo(left);

			}
			virtual bool empty() const
			{
				
				return _This->empty();

			}

			Data& operator=(const Data& left)
			{
				if (_This) _This.reset();

				_This = left._This;
				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}

			void release()
			{
				if (_This)
				{
					_This.reset();
					
				}
			}

		protected:
			Data(IODataType datatype) : _type(datatype)
			{
				_decorate = false;
			}

			void setType(const IODataType io_data_type)
			{
				_type = io_data_type;
			}
		};

		/**
		 * \brief [TODO]
		 * \tparam Base 
		 * \tparam Derived 
		 */
		template <typename Base, typename Derived>
		class IOData : public Base
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

				return static_cast<const Derived&>(This_const()).empty();

			}

			IODataType getType() const
			{
				return Base::getType();
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

			inline Derived & operator=(const Data& left)
			{
				Data::registerInstance(left);
				Data::_type = left.getType();
				Data::_decorate = left.getDecorate();
				
				return This();
			}

		/*	IOData& operator<<(const IOData& left)
			{
				if (_type != left._type)
					throw HipeException("Cannot add data because types are different");

				throw HipeException("Not yet impelmented check if the  pointer to return is this or left");

				return *this;
			}*/

			
		};
	}
}
