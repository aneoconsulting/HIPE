#pragma once
#include <vector>
#include <data/Data.h>
#include <stack>
#include <core/queue/ConcurrentQueue.h>
#include <boost/python.hpp>
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
