//%HIPE_LICENSE%
#include <algos/preprocessing/Ofuscator.h>


bool filter::algos::Obfuscator::isShapeData(const data::Data& input)
{
	if (input.getType() == data::SHAPE)
	{
		return true;
	}

	return false;
}

HipeStatus filter::algos::Obfuscator::process()
{
	if (_connexData.empty())
		return OK;

	// Separate shapes from source image
	std::vector<data::ShapeData> shapes;
	bool isFound = false;
	data::ImageData image;

	while (!_connexData.empty())
	{
		data::Data data = _connexData.pop();
		if (data.getType() == data::SHAPE)
		{
			data::ShapeData shape = static_cast<data::ShapeData &>(data);

			shapes.push_back(shape);
		}
		else if (data.getType() == data::IMGF)
		{
			if (isFound)
				throw HipeException("Cannot manage multiple image source. Please Duplicate Node obfuscator instead");

			image = static_cast<data::ImageData &>(data);

			isFound = true;
						
		}
		
	}

	if (!isFound)
		throw HipeException("No input frame received in Obfuscator");

	cv::Mat & largeImg = image.getMat();
	if ( ! largeImg.empty())
	{
		for (auto & shape : shapes)
		{
			for (cv::Rect crop : shape.RectsArray())
			{
				cv::Mat roi(largeImg, crop);

				if (roi.cols >= 3 && roi.rows >= 3)
				{
					// apply whatever algo on 'roi'
					blur( roi, roi, cv::Size(3,3) );
					blur( roi, roi, cv::Size(3,3) );
					blur( roi, roi, cv::Size(3,3) );
					GaussianBlur( roi, roi, cv::Size(45,45), 0);
				}
			}
		}
	}

	PUSH_DATA(image);

	return OK;
}
