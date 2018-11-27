//@HIPE_LICENSE@
#include <filter/algos/preprocessing/OverlayFilter.h>

HipeStatus filter::algos::OverlayFilter::process()
{
	// Assert data is present in connector
	if (_connexData.empty())
	{
		return OK;
	}

	// Separate shapes from source image
	std::vector<data::Data> overlayData;
	data::ImageData image;
	bool isSourceFound = false;

	while (!_connexData.empty())
	{
		data::Data data = _connexData.pop();
		if (isOverlayData(data))
		{
			overlayData.push_back(data);
		}
		else if (isDrawableSource(data))
		{
			if (isSourceFound)
				throw HipeException(
					"Error in OverlayFilter: Overlay filter only accepts one input source to draw on. Other input data should only be ShapeData objects.");
			isSourceFound = true;

			image = extractSourceImageData(data);
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "Error in OverlayFilter: Input type is not handled: ";
			errorMessage << data::DataTypeMapper::getStringFromType(data.getType());
			errorMessage << std::endl;
			throw HipeException(errorMessage.str());
		}
	}

	if (image.empty() || !image.getMat().data)
		return OK;

	cv::Mat outputImage;

	if (! asReference)
		outputImage = image.getMat().clone();
	else
		outputImage = image.getMat();

	for (auto overlayDatum : overlayData)
	{
		if (overlayDatum.getType() == data::SHAPE) drawShape(outputImage, static_cast<data::ShapeData &>(overlayDatum));
	}

	data::ImageData image_data = data::ImageData(outputImage);
	PUSH_DATA(image_data);

	return OK;
}

bool filter::algos::OverlayFilter::isDrawableSource(const data::Data& data)
{
	switch (data.getType())
	{
	case data::IMGF:
	case data::PATTERN:
	case data::DIRPATTERN:
		return true;
	default:
		return false;
	}
}

bool filter::algos::OverlayFilter::isOverlayData(const data::Data& data)
{
	switch (data.getType())
	{
	case data::SHAPE:
		return true;
	case data::TXT:
	case data::TXT_ARR:
		{
			std::stringstream errorMessage;
			errorMessage << "Error in OverlayFilter: The overlay filter can't display text ATM.\n";
			errorMessage << "\t found type: " << data::DataTypeMapper::getStringFromType(data.getType());
			throw HipeException(errorMessage.str());
		}
	default:
		return false;
	}
}

data::ImageData filter::algos::OverlayFilter::extractSourceImageData(data::Data& data)
{
	const data::IODataType& type = data.getType();
	if (type == data::IMGF)
	{
		return static_cast<data::ImageData &>(data);
	}
	else if (type == data::PATTERN)
	{
		data::PatternData pattern = static_cast<data::PatternData &>(data);
		return pattern.imageRequest();
	}
		// Now we can extract source image directly from dirpatterndata
	else if (type == data::DIRPATTERN)
	{
		data::DirPatternData& dirPattern = static_cast<data::DirPatternData &>(data);
		return dirPattern.imageSource();
	}
	else
	{
		return data::ImageData();
	}
}


size_t filter::algos::OverlayFilter::computeSizeOfAllText(std::string text)
{
	size_t size = 0;
	int baseline = 0;

	cv::Size text_size = cv::getTextSize(text, cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontScale, thickness, &baseline);
	size += text_size.height + 10;
	return size;
}

void filter::algos::OverlayFilter::drawShape(cv::Mat& image, const data::ShapeData& shape)
{
	const cv::Scalar pointsColor(0, 255, 255);
	const cv::Scalar circlesColor(0, 255, 0);
	const cv::Scalar rectsColor(255, 0, 0);
	const cv::Scalar quadsColor(255, 0, 255);

	// Draw points
	for (const cv::Point2f& point : shape.PointsArray_const())
	{
		cv::circle(image, point, 2, pointsColor, 2);
	}

	// Draw circles
	for (const cv::Vec3f& circle : shape.CirclesArray_const())
	{
		cv::Point2f center(circle[0], circle[1]);
		const float radius = circle[2];
		cv::circle(image, center, radius, circlesColor, thickness);
	}

	unsigned int nbColor = shape.ColorsArray_const().size();
	int k = 0;
	// Draw rects
	for (const cv::Rect& rect : shape.RectsArray_const())
	{
		cv::Scalar color;

		if ( nbColor != 0)
			color = shape.ColorsArray_const()[k % nbColor];
		else
			color = cv::Scalar(255, 255, 255);

		cv::rectangle(image, rect, color, thickness);
		k++;
	}

	// Draw quads
	for (const data::four_points& quad : shape.QuadrilatereArray_const())
	{
		for (size_t i = 0; i < quad.size() - 1; ++i)
		{
			cv::line(image, quad[i], quad[i + 1], quadsColor, thickness);
		}

		cv::line(image, quad.front(), quad.back(), quadsColor, thickness);
	}

	// Draw freeShapes
	for (int i = 0; i < shape.FreeshapeArray_const().size(); i++)
	{
		const std::vector<cv::Point>& freeShape = shape.FreeshapeArray_const()[i];

		cv::Scalar color;

		if ( nbColor != 0)
			color = shape.ColorsArray_const()[i % nbColor];
		else
			color = cv::Scalar(0, 147, 245); //ANEO COLOR

		cv::polylines(image, freeShape, false, color, thickness, 16);
	}

	unsigned int nbRect = shape.RectsArray_const().size();
	
	for (int i = 0; i < shape.IdsArray_const().size(); i++)
	{
		std::string text = shape.IdsArray_const()[i];

		cv::Rect rect;
		if (shape.RectsArray_const().size() != 0)
			rect = shape.RectsArray_const()[i % nbRect];
		else
			rect = cv::Rect(0,0, 1, 1);

		cv::Scalar color;

		if ( nbColor != 0)
			color = shape.ColorsArray_const()[i % nbColor];
		else
			color = cv::Scalar(255, 255, 255);

		unsigned int height_text = static_cast<unsigned int>(computeSizeOfAllText(text));

		if (rect.y < height_text)
			cv::putText(image, text, cv::Point(rect.x, height_text + 10),
		            cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontScale, color, thickness);
		else
			cv::putText(image, text, cv::Point(rect.x, rect.y >= 3 ? rect.y - 3 : rect.y),
		            cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontScale, color, thickness);
	}
}
