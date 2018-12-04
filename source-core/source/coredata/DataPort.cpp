//@HIPE_LICENSE@
#include <coredata/DataPort.h>

namespace data
{
	Data DataPort::get()
	{
		return pop();
	}

	Data DataPort::pop()
	{
		Data value;
		if (data.pop(value) == false)
			throw HipeException("No more data to pop from the dataPort");

		return value;
	}

	void DataPort::push(Data& dataIn)
	{
		data.push(dataIn);
	}

	bool DataPort::empty()
	{
		return data.empty();
	}

	size_t DataPort::size()
	{
		return data.size();
	}
}