#include <coredata/Data.h>

namespace data
{
	Data::Data(IODataType datatype): _type(datatype)
	{
		_decorate = false;
	}

	Data::Data(): _type(NONE), _decorate(false)
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
