#include <filter/algos/OverlayFilter.h>

HipeStatus filter::algos::OverlayFilter::process()
{
	// Assert data is present in connector
	if (_connexData.empty())
	{
		throw HipeException("Error in OverlayFilter: No data in input.");
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
				throw HipeException("Error in OverlayFilter: Overlay filter only accepts one input source to draw on. Other input data should only be ShapeData objects.");
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
		throw HipeException("Error in OverlayFilter: No input image to draw on found.");

	cv::Mat outputImage = image.getMat().clone();
	for (auto overlayDatum : overlayData)
	{
		if (overlayDatum.getType() == data::SHAPE) drawShape(outputImage, static_cast<data::ShapeData &>(overlayDatum));
	}

	_connexData.push(data::ImageData(outputImage));

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

filter::data::ImageData filter::algos::OverlayFilter::extractSourceImageData(data::Data& data)
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
		data::DirPatternData & dirPattern = static_cast<data::DirPatternData &>(data);
		return dirPattern.imageRequest();
	}
	else
	{
		return data::ImageData();
	}
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
		cv::circle(image, center, radius, circlesColor, 2);
	}

	// Draw rects
	for (const cv::Rect& rect : shape.RectsArray_const())
	{
		cv::rectangle(image, rect, rectsColor, 2);
	}

	// Draw quads
	for (const data::four_points& quad : shape.QuadrilatereArray_const())
	{
		for (size_t i = 0; i < quad.size() - 1; ++i)
		{
			cv::line(image, quad[i], quad[i + 1], quadsColor, 2);
		}

		cv::line(image, quad.front(), quad.back(), quadsColor, 2);
	}
}
