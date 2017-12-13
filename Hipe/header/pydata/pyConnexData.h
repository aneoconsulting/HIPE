#pragma once
#include <data/IOData.h>
#include <queue>
#include "data/ConnexData.h"
#include "pyData.h"


class pyConnexData
{
	filter::data::ConnexDataBase& _connexData;

public:
	pyConnexData(filter::data::ConnexDataBase& connector) : _connexData(connector)
	{
	}

	boost::shared_ptr<pyData> pop()
	{

		return boost::shared_ptr<pyData>();
	}

	void push(pyData *data)
	{
		
	}
};
