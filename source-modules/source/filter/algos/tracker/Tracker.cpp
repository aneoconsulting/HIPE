//@HIPE_LICENSE@
#if defined(USE_DLIB) &&  defined(__ALTIVEC__)
	//issue order of header for vector keyword call it before 
	#include <dlib/simd.h>
#endif
#include <filter/algos/tracker/Tracker.h>
#include <data/ImageData.h>
#include <data/PatternData.h>
#include <data/DirPatternData.h>


#pragma warning(push, 0)
#include <dlib/opencv/cv_image.h>
#pragma warning(pop)

class Util
{
public:

	/* --------------------------------------------
	Function : cvtRectToRect
	Convert cv::Rect to dlib::drectangle
	----------------------------------------------- */
	static dlib::drectangle cvtRectToDrect(cv::Rect _rect)
	{
		return dlib::drectangle(_rect.tl().x, _rect.tl().y, _rect.br().x - 1, _rect.br().y - 1);
	}


	/* -------------------------------------------------
	Function : cvtMatToArray2d
	convert cv::Mat to dlib::array2d<unsigned char>
	------------------------------------------------- */
	static dlib::array2d<unsigned char> cvtMatToArray2d(cv::Mat _mat) // cv::Mat, not cv::Mat&. Make sure use copy of image, not the original one when converting to grayscale
	{
		//Don't need to use color image in HOG-feature-based tracker
		//Convert color image to grayscale
		if (_mat.channels() == 3)
			cv::cvtColor(_mat, _mat, cv::COLOR_RGB2GRAY);

		//Convert opencv 'MAT' to dlib 'array2d<unsigned char>'
		dlib::array2d<unsigned char> dlib_img;
		dlib::assign_image(dlib_img, dlib::cv_image<unsigned char>(_mat));

		return dlib_img;
	}


	/* -----------------------------------------------------------------
	Function : setRectToImage
	Put all tracking results(new rectangle) on the frame image
	Parameter _rects is stl container(such as vector..) filled with
	cv::Rect
	----------------------------------------------------------------- */
	template <typename Container>
	static void setRectToImage(cv::Mat& _mat_img, Container _rects)
	{
		std::for_each(_rects.begin(), _rects.end(), [&_mat_img](cv::Rect rect)
	              {
		              cv::rectangle(_mat_img, rect, cv::Scalar(0, 0, 255));
	              });
	}
};

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

filter::algos::SingleTracker::SingleTracker(int id, cv::Mat& img, const cv::Rect rect): targetId(id)
{
	dlib::array2d<unsigned char> cvMatToArray2d = Util::cvtMatToArray2d(img);
	tracker.start_track(cvMatToArray2d, Util::cvtRectToDrect(rect));
}

double filter::algos::SingleTracker::update_noscale(const cv::Mat& mat)
{
	dlib::array2d<unsigned char> cvMatToArray2d = Util::cvtMatToArray2d(mat);
	return tracker.update_noscale(cvMatToArray2d);
}

HipeStatus filter::algos::Tracker::process()
{
	// Assert data is present in connector
	if (_connexData.empty())
	{
		throw HipeException("Error in Tracker: No data in input.");
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
		else if (isDrawableSource(data))
		{
			if (isSourceFound)
				throw HipeException(
					"Error in Tracker: Tracker filter only accepts one input source to draw on. Other input data should only be ShapeData objects.");
			isSourceFound = true;

			image = extractSourceImageData(data);
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "Error in Tracker: Input type is not handled: ";
			errorMessage << data::DataTypeMapper::getStringFromType(data.getType());
			errorMessage << std::endl;
			throw HipeException(errorMessage.str());
		}
	}

	if (! isSourceFound || image.empty() || !image.getMat().data)
		throw HipeException("Error in Tracker: No input image to draw on found.");

	if (shapes[0].RectsArray_const().empty())
	{
		PUSH_DATA(data::ShapeData());
		return OK;
	}
	auto rect = shapes[0].RectsArray_const()[0];

	if (!_init.exchange(true))
	{
		_id++;
		trackers.clear();
		trackers.push_back(SingleTracker(_id, image.getMat(), rect));
		
	}


	double l_confidence = trackers[0].update_noscale(image.getMat());
	dlib::drectangle drectangle = trackers[0].tracker.get_position();
	auto corner = drectangle.tl_corner();
	cv::Point center = cv::Point(corner.x() + (drectangle.width() / 2),
	                             corner.y() + (drectangle.height() / 2));

	bool is_target_inside_frame = isTargetInsideFrame(image.getMat(), center);
	if (l_confidence >= confidence && is_target_inside_frame)
	{
		auto& history_track = trackers[0].history_track;
		if (center.x < rect.x + rect.width / 4 || center.x > rect.x + 3 * (rect.width / 4) ||
			center.y < rect.y + rect.height / 4 || center.y > rect.y + 3 * (rect.height / 4) )
		{
			trackers.clear();
			trackers.push_back(SingleTracker(_id, image.getMat(), rect));
			l_confidence = trackers[0].update_noscale(image.getMat());
			drectangle = trackers[0].tracker.get_position();
			corner = drectangle.tl_corner();
			center = cv::Point(corner.x() + (drectangle.width() / 2),
				corner.y() + (drectangle.height() / 2));
		}

		history_track.push_back(center);
		if (history_track.size() > history_points)
		{
			history_track.pop_front();
		}

		data::ShapeData result;
		std::vector<cv::Point> vec(history_track.begin(), history_track.end());
		result.add(vec);
		result.add(cv::Rect(corner.x(), corner.y(), drectangle.width(), drectangle.height()));

		PUSH_DATA(result);
	}
	else
	{
		trackers.clear();
		//if (is_target_inside_frame)
		//{
		//	trackers.push_back(SingleTracker(_id, image.getMat(), shapes[0].RectsArray_const()[0]));
		//}
		//else
		//{
		_init.exchange(false);
		//}
		PUSH_DATA(data::ShapeData());
	}

	return OK;
}
