#include <filter/algos/tracker/IfStableTracking.h>
#include <data/ImageData.h>
#include <data/PatternData.h>
#include <data/DirPatternData.h>


#pragma warning(push, 0)
#include <dlib/opencv/cv_image.h>
#pragma warning(pop)

inline bool isShapeData(const data::Data& data)
{
	switch (data.getType())
	{
	case data::SHAPE:
		return true;
	default:
		return false;
	}
}

inline bool isDrawableSource(const data::Data& data)
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

inline data::ImageData extractSourceImageData(data::Data& data)
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

inline bool isTargetInsideFrame(const cv::Mat& image, const cv::Point& center)
{
	int cur_x = center.x;
	int cur_y = center.y;

	bool is_x_inside = ((0 <= cur_x) && (cur_x < image.size().width));
	bool is_y_inside = ((0 <= cur_y) && (cur_y < image.size().height));

	if (is_x_inside && is_y_inside)
		return true;
	else
		return false;
}


HipeStatus filter::algos::IfStableTracking::process()
{
	// Assert data is present in connector
	if (_connexData.empty())
	{
		throw HipeException("Error in IfStableTracking: No data in input.");
	}

	// Separate shapes from source image
	std::vector<data::ShapeData> shapes;
	data::ImageData image;
	bool isSourceFound = false;

	while (!_connexData.empty())
	{
		data::Data data = _connexData.pop();
		if (isShapeData(data))
		{
			shapes.push_back(static_cast<data::ShapeData &>(data));
		}
		
	}

	//if (! isSourceFound || image.empty() || !image.getMat().data)
	//	throw HipeException("Error in IfStableTracking: No input image to draw on found.");
	std::vector<std::vector<cv::Point> > & vec_point = shapes[0].FreeshapeArray();
	if (shapes[0].empty() || vec_point.size() == 0 || vec_point[0].size() < 10)
	{
		PUSH_DATA(data::ShapeData());
		return OK;
	}
	
	
	std::vector<cv::Point> & points = vec_point[0];
	float avg_x = 0.f;
	float avg_y = 0.f;
	float min_x = std::numeric_limits<float>::max();
	float min_y = std::numeric_limits<float>::max();

	cv::Point2f point_prev = points[0];

	for(int i = 1; i < points.size(); i++)
	{
		cv::Point2f point = points[i];
		if ((float)point.x < min_x) min_x = (float)point.x;
		if ((float)point.y < min_y) min_y = (float)point.y;

		avg_x += ((float)point.x - (float)point_prev.x);
		avg_y += (float)point.y - (float)point_prev.y;

		point_prev = points[i];
	}
	if (!points.empty())
	{
		avg_x = std::abs(avg_x / (float)points.size());
		avg_y = std::abs(avg_y / (float)points.size());

		if (avg_x < confidence && avg_y < confidence)
		{
			data::ShapeData result;
			result.add(shapes[0].RectsArray()[0]);
			PUSH_DATA(result);
			return OK;
		}
	}

	data::ShapeData result;

	PUSH_DATA(result);
	
	return OK;
}
