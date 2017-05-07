#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include "IODataType.h"

namespace filter
{
	namespace data
	{
		class Data
		{
		public:
		public:
			typedef Data _classtype;
		protected:
			IODataType _type;
			std::shared_ptr<Data> _This;

		
		public:
			Data() : _type(NONE)//, This(std::make_shared<Data>(nullptr))
			{
				
			}

			Data(const Data& data) : _type(data._type), _This(data._This)
			{

			}


			virtual ~Data()
			{
			}

			inline void registerInstance(Data * childInstance)
			{
				_This.reset(childInstance);
			}

			inline void registerInstance(std::shared_ptr<Data> childInstance)
			{
				_This = childInstance;
			}


			IODataType getType() const
			{
				return _type;
			}

			void copyTypeTo(Data& left)
			{
				if (left._type != _type && left._type != NONE)
					throw HipeException("Cannot copy type [ONE] to type [TWO]");

				left._type = _type; // case where left._type == NONE
			}

			virtual void copyTo(Data& left)
			{
				
				_This->copyTo(left);

			}

			Data& operator=(const Data& left)
			{
				_This = left._This;
				_type = left._type;
				return *this;
			}

		protected:
			Data(IODataType datatype) : _type(datatype)
			{
				
			}


			void setType(const IODataType io_data_type)
			{
				_type = io_data_type;
			}
		};

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
			virtual ~IOData()
			{
			}

			virtual void copyTo(IOData& left)
			{
				static_cast<Derived&>(*(Base::_This)).copyTo(static_cast<Derived&>(left));
			}

			

			IODataType getType() const
			{
				return Base::getType();
			}

			inline Derived &  This() const
			{
				return static_cast<Derived &>(*((Base::_This).get()));
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
