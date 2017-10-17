#pragma once

#include <data/IODataType.h>
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

	protected:
		Data(IODataType datatype) : _type(datatype)
		{
			_decorate = false;
		}

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

		inline void registerInstance(const Data & childInstance);

		inline void registerInstance(Data* childInstance);

		inline void registerInstance(std::shared_ptr<Data> childInstance);


		inline IODataType getType() const;

		inline bool getDecorate() const;

		void copyTypeTo(Data& left);

		virtual void copyTo(Data& left) const;

		virtual bool empty() const;

		Data& operator=(const Data& left);

		inline void release();

		inline void setType(const IODataType io_data_type);
	};
}
