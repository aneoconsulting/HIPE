//@HIPE_LICENSE@
#include <filter/algos/preprocessing/ReshapeImage.h>

#pragma warning(push, 0)
#include <opencv2/video/tracking.hpp>
#pragma warning(pop)

HipeStatus filter::algos::ReshapeImage::process()
{
	if (_connexData.size() != 2)
		throw HipeException("ERROR - filter::algos::ReshapeImage::process - wrong input data count, expected one image and one shape");

	data::Data imageData = _connexData.pop();
	data::Data shapeData = _connexData.pop();

	if (imageData.getType() != data::IODataType::IMGF)
		std::swap(imageData, shapeData);

	const data::IODataType& imageType = imageData.getType();
	const data::IODataType& shapeType = shapeData.getType();

	if (imageType != data::IODataType::IMGF || shapeType != data::IODataType::SHAPE)
	{
		std::stringstream errorMessage;
		errorMessage <<
			"ERROR - filter::algos::ReshapeData::process - wrong input data types: "
			<< data::DataTypeMapper::getStringFromType(imageType)
			<< ", " << data::DataTypeMapper::getStringFromType(shapeType)
			<< ") expected image (IMGF) ang shape (SHAPE).";
		throw HipeException(errorMessage.str());
	}

	const std::vector<cv::Vec3f>& circles = static_cast<data::ShapeData&>(shapeData).CirclesArray_const();
	const cv::Mat& image = static_cast<data::ImageData>(imageData).getMat();

	cv::Mat outputImage = reshapeImage(image, circles, cv::Size(target_image_width, target_image_height), cv::Size(target_grid_width, target_grid_height));

	PUSH_DATA(data::ImageData(outputImage));

	return OK;
}

cv::Mat filter::algos::ReshapeImage::reshapeImage(const cv::Mat& image, const std::vector<cv::Vec3f>& circles, const cv::Size& target_size, const cv::Size& expected_grid_size)
{
	int x_min = 0,
		y_min = 0,
		x_max = image.cols - 1,	//current_size.width - 1,
		y_max = image.rows - 1;	//current_size.height - 1;

								// Target image parameters.
	int tx_spacing, tx_margin, tx_min, tx_max,
		ty_spacing, ty_margin, ty_min, ty_max;

	std::vector<cv::Vec2i> corners{ { x_min,y_min },{ x_min,y_max },{ x_max,y_min },{ x_max,y_max } };
	std::vector<unsigned int> shortest(4, UINT_MAX);

	//! Source points for the transform.
	std::vector<cv::Point2f> src(4);

	// Use closest circle to each corner as a source point.
	for (unsigned int i = 0; i < circles.size(); ++i)
	{
		for (unsigned int j = 0; j < 4; ++j)
		{
			int dx = corners[j][0] - circles[i][0],
				dy = corners[j][1] - circles[i][1];
			unsigned dr = dx*dx + dy*dy;
			if (dr < shortest[j])
			{
				shortest[j] = dr;
				src[j] = cv::Point2f(circles[i][0], circles[i][1]);
			}
		}
	}

	/*
	Calculate the destination points. These are the corners of a regularly
	aligned grid of n*n element such that the distance of each corner to the
	edge is half the distance to the nearest neighboring point.
	*/
	tx_spacing = target_size.width / expected_grid_size.width; // cols = 4
	ty_spacing = target_size.height / expected_grid_size.height; // rows = 4
	tx_margin = tx_spacing / 2;
	ty_margin = ty_spacing / 2;
	tx_min = tx_margin;
	ty_min = ty_margin;
	tx_max = target_size.width - tx_margin;
	ty_max = target_size.height - ty_margin;

	//! Destination points for the transform.
	std::vector<cv::Point2f> dst = {
		cv::Point2f(tx_min,ty_min),
		cv::Point2f(tx_min,ty_max),
		cv::Point2f(tx_max,ty_min),
		cv::Point2f(tx_max,ty_max)
	};

	cv::Mat transformMatrix = cv::getPerspectiveTransform(src, dst);
	cv::Mat reshapedImage;
	cv::warpPerspective(image, reshapedImage, transformMatrix, target_size);

	return reshapedImage;
}
