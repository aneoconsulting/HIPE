//@HIPE_LICENSE@
#pragma once
#include <core/queue/ConcurrentQueue.h>

#include <pydata/pyData.h>

class pyFilter
{
	core::queue::ConcurrentQueue<pydata::pyData> connexData;

	void py_push(pydata::pyData &toPush)
	{
		//connexData.push(toPush);
	}


	void push(pydata::pyData &toPush)
	{
		connexData.push(toPush);
	}

	pydata::pyData pop()
	{
		pydata::pyData res;
		connexData.try_pop(res);
		return res;
	}

};
