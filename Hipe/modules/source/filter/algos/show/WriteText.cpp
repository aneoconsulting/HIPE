#include <filter/algos/show/WriteText.h>
#include <opencv2/videostab/wobble_suppression.hpp>
#include "algos/utils/CVUtils.h"

HipeStatus filter::algos::WriteText::process()
{
	std::vector<data::ImageArrayData> arrayData;

	while (!_connexData.empty())
	{
		data::Data img = _connexData.pop();
		
		if (img.getType() == data::IODataType::IMGF)
		{
			data::ImageData & image_data = static_cast<data::ImageData&>(img);
			arrayData.push_back(image_data);
		}
		else if (!data::DataTypeMapper::isSequenceDirectory(img.getType()) && data::DataTypeMapper::isImage(img.getType()))
		{
			data::ImageArrayData image_data = static_cast<data::ImageArrayData>(img);
			arrayData.push_back(image_data);
		}
	}
	

	for (data::ImageArrayData listOfArray : arrayData)
	{
		data::ImageArrayData res;

		for (cv::Mat image : listOfArray.Array())
		{
			res.Array().push_back(CVUtils::writeTextOnImage(image, text));
		}
		PUSH_DATA(res);
	}
	

	


	return OK;
}
