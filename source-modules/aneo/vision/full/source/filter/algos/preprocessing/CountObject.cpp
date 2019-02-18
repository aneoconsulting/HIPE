//%HIPE_LICENSE%
#include <algos/preprocessing/CountObject.h>


bool filter::algos::CountObject::isShapeData(const data::Data& input)
{
	if (input.getType() == data::SHAPE)
	{
		return true;
	}

	return false;
}

HipeStatus filter::algos::CountObject::process()
{
	if (_connexData.empty())
		return OK;
	if (!isStart.exchange(true))
	{
		timeout.start();
	}
	// Separate shapes from source image
	std::vector<data::ShapeData> shapes;

	while (!_connexData.empty())
	{
		data::ShapeData shape = _connexData.pop();
		if (shape.getType() == data::SHAPE)
		{
			shapes.push_back(shape);
		}
	}
	std::map<std::string, size_t> res;

	for (auto & shape : shapes)
	{
			for (std::string crop : shape.IdsArray())
			{
				std::map<std::string, size_t>::iterator pair = res.find(crop);
				
				if (pair == res.end()) res[crop] = 1;
				else
				{
					size_t inc = pair->second;
					res[crop] = inc + 1;
				}
			}
		
	}
	bool isTimeout = false;

	if (timeout.stop().getElapseTimeInMili() > recurrent)
	{
		isTimeout = true;
		countTable.clear();

	}
	for (auto pair : res)
	{
		if (countTable.find(pair.first) != countTable.end())
		{
			if (isTimeout)
			{
				countTable[pair.first] = pair.second;
			}
		
		}
	}
	data::ShapeData data;
	if (isTimeout)
	{
		timeout.start();
		
		for (auto pair : res)
		{
			std::stringstream ids;
			ids << pair.second << " " << pair.first;
			data.IdsArray().push_back(ids.str());
		}
	}

	PUSH_DATA(data);


	

	return OK;
}
