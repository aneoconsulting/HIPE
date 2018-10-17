//@HIPE_LICENSE@
#if defined(USE_DLIB) &&  defined(__ALTIVEC__)
//issue order of header for vector keyword call it before 
	#include <dlib/simd.h>
#endif
#include <filter/algos/tracker/tracker.h>
#include <data/ImageData.h>
#include <data/PatternData.h>
#include <data/DirPatternData.h>


#pragma warning(push, 0)
#include <dlib/opencv/cv_image.h>
#include <glog/logging.h>
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

cv::Rect filter::algos::SingleTracker::get_position()
{
	const dlib::drectangle & location = tracker->get_position();
	auto & corner = location.tl_corner();
	_rect = cv::Rect(corner.x(), corner.y(), location.width(), location.height());

	return _rect;
}

filter::algos::SingleTracker::SingleTracker(int id, cv::Mat& img, const cv::Rect rect, std::string & name) :
	targetId(id), _rect(rect), _name(name)
{
	tracker = new dlib::correlation_tracker(6);

	dlib::array2d<unsigned char> cvMatToArray2d = Util::cvtMatToArray2d(img);
	tracker->start_track(cvMatToArray2d, Util::cvtRectToDrect(rect));
}

filter::algos::SingleTracker::SingleTracker(const SingleTracker& left)
{
	tracker = new dlib::correlation_tracker(6);
	targetId = left.targetId;
	_rect = left._rect;
	_name = left._name;
}

filter::algos::SingleTracker::~SingleTracker()
{
	if (tracker != nullptr)
	{
		delete tracker;
		tracker = nullptr;
	}
}


double filter::algos::SingleTracker::update(const cv::Mat& mat) const
{
	dlib::array2d<unsigned char> cvMatToArray2d = Util::cvtMatToArray2d(mat);
	return tracker->update(cvMatToArray2d);
}

double filter::algos::SingleTracker::update(const cv::Mat& mat, cv::Rect & rect)
{
	dlib::drectangle dRect = Util::cvtRectToDrect(rect);
	_rect = rect;
	dlib::array2d<unsigned char> cvMatToArray2d = Util::cvtMatToArray2d(mat);
	return tracker->update(cvMatToArray2d, dRect);
}

double filter::algos::SingleTracker::update_noscale(const cv::Mat& mat)
{
	dlib::array2d<unsigned char> cvMatToArray2d = Util::cvtMatToArray2d(mat);
	return tracker->update_noscale(cvMatToArray2d);
}

double filter::algos::SingleTracker::update_noscale(const cv::Mat& mat, cv::Rect & rect)
{
	dlib::drectangle dRect = Util::cvtRectToDrect(rect);
	_rect = rect;
	dlib::array2d<unsigned char> cvMatToArray2d = Util::cvtMatToArray2d(mat);
	return tracker->update_noscale(cvMatToArray2d, dRect);
}

HipeStatus old_process()
{
	//if (_connexData.empty())
	//{
	//	throw HipeException("Error in Tracker: No data in input.");
	//}

	//// Separate shapes from source image
	//std::vector<data::ShapeData> shapes;
	//data::ImageData image;
	//bool isSourceFound = false;

	//while (!_connexData.empty())
	//{
	//	data::Data data = _connexData.pop();
	//	if (isShapeData(data))
	//	{
	//		shapes.push_back(static_cast<data::ShapeData &>(data));
	//	}
	//	else if (isDrawableSource(data))
	//	{
	//		if (isSourceFound)
	//			throw HipeException(
	//				"Error in Tracker: Tracker filter only accepts one input source to draw on. Other input data should only be ShapeData objects.");
	//		isSourceFound = true;

	//		image = extractSourceImageData(data);
	//	}
	//	else
	//	{
	//		std::stringstream errorMessage;
	//		errorMessage << "Error in Tracker: Input type is not handled: ";
	//		errorMessage << data::DataTypeMapper::getStringFromType(data.getType());
	//		errorMessage << std::endl;
	//		throw HipeException(errorMessage.str());
	//	}
	//}

	//if (!isSourceFound || image.empty() || !image.getMat().data)
	//	throw HipeException("Error in Tracker: No input image to draw on found.");
	////Need one shape to init otherwise go out for your way
	//if ((shapes.size() == 0 || shapes[0].RectsArray_const().empty()) && _init == false)
	//{
	//	PUSH_DATA(data::ShapeData());
	//	return OK;
	//}


	//if (!_init.exchange(true))
	//{
	//	auto rect = shapes[0].RectsArray_const()[0];
	//	_id++;
	//	trackers.clear();
	//	trackers.push_back(SingleTracker(_id, image.getMat(), rect));

	//}

	//for (SingleTracker track : trackers)
	//{
	//	double l_confidence = track.update_noscale(image.getMat());
	//	dlib::drectangle drectangle = track.tracker->get_position();
	//	auto corner = drectangle.tl_corner();
	//	cv::Point center = cv::Point(corner.x() + (drectangle.width() / 2),
	//		corner.y() + (drectangle.height() / 2));

	//	bool is_target_inside_frame = isTargetInsideFrame(image.getMat(), center);

	//	//Ok the tracker has identified the object with a correct confidence
	//	if (l_confidence >= confidence && is_target_inside_frame)
	//	{
	//		//if there is a rectangle in input check to recalibrate the tracker
	//		// Need to check if it's an update of the Tracker estimation
	//		if (shapes.size() != 0 && !shapes[0].RectsArray_const().empty())
	//		{
	//			for (const cv::Rect rect : shapes[0].RectsArray_const())
	//			{
	//				//Fail if object has moved too fast or if it's another object
	//				if (center.x < rect.x + rect.width / 4 || center.x > rect.x + 3 * (rect.width / 4) ||
	//					center.y < rect.y + rect.height / 4 || center.y > rect.y + 3 * (rect.height / 4))
	//				{
	//				}
	//			}
	//		}
	//	}
	//}


	//if (l_confidence >= confidence && is_target_inside_frame)
	//{
	//	auto& history_track = trackers[0].history_track;
	//	if (center.x < rect.x + rect.width / 4 || center.x > rect.x + 3 * (rect.width / 4) ||
	//		center.y < rect.y + rect.height / 4 || center.y > rect.y + 3 * (rect.height / 4))
	//	{
	//		trackers.clear();
	//		trackers.push_back(SingleTracker(_id, image.getMat(), rect));
	//		l_confidence = trackers[0].update_noscale(image.getMat());
	//		drectangle = trackers[0].tracker->get_position();
	//		corner = drectangle.tl_corner();
	//		center = cv::Point(corner.x() + (drectangle.width() / 2),
	//			corner.y() + (drectangle.height() / 2));
	//	}

	//	history_track.push_back(center);
	//	if (history_track.size() > history_points)
	//	{
	//		history_track.pop_front();
	//	}

	//	data::ShapeData result;
	//	std::vector<cv::Point> vec(history_track.begin(), history_track.end());
	//	result.add(vec);
	//	result.add(cv::Rect(corner.x(), corner.y(), drectangle.width(), drectangle.height()));

	//	PUSH_DATA(result);
	//}
	//else
	//{
	//	trackers.clear();
	//	//if (is_target_inside_frame)
	//	//{
	//	//	trackers.push_back(SingleTracker(_id, image.getMat(), shapes[0].RectsArray_const()[0]));
	//	//}
	//	//else
	//	//{
	//	_init.exchange(false);
	//	//}
	//	PUSH_DATA(data::ShapeData());
	//}

	return OK;
}

bool hasIdAndRect(const std::vector<data::ShapeData>& shapes)
{
	if (shapes.size() == 0)
	{
		return false;
	}

	for (data::ShapeData shapeData : shapes)
	{
		if(shapeData.IdsArray().size() != 0 && shapeData.RectsArray_const().size() == shapeData.IdsArray().size())
		{
			return true;
		}
	}

	return false;
}

void getIdsAndRect(const std::vector<data::ShapeData>& shapes, 
	std::vector<std::string>& ids, 
	std::vector<cv::Rect_<int>>& rects,
	std::vector<cv::Mat> & new_rectImgs)
{
	if (shapes.size() == 0)
	{
		return;
	}

	for (data::ShapeData shapeData : shapes)
	{
		cv::Mat gray;
		if (! shapeData.RefFrame().empty())
		{
			cv:cvtColor(shapeData.RefFrame(), gray, CV_RGB2GRAY);
		}

		if (shapeData.IdsArray().size() != 0 && shapeData.RectsArray_const().size() == shapeData.IdsArray().size())
		{
			for (int i = 0; i < shapeData.IdsArray_const().size(); i++)
			{
				ids.push_back(shapeData.IdsArray_const()[i]);
				rects.push_back(shapeData.RectsArray_const()[i]);
				new_rectImgs.push_back(gray);
			}
		}
	}
}

void filter::algos::Tracker::clearTrackerOutsideFrame(const cv::Mat & frame)
{
	std::vector<SingleTracker *> newTrackers;

	for (SingleTracker * track : trackers)
	{
		double l_confidence = track->update(frame);
		dlib::drectangle drectangle = track->tracker->get_position();
		auto corner = drectangle.tl_corner();
		cv::Point center = cv::Point(corner.x() + (drectangle.width() / 2),
			corner.y() + (drectangle.height() / 2));

		bool is_target_inside_frame = isTargetInsideFrame(frame, center);

		if (l_confidence >= confidence && is_target_inside_frame)
		{
			newTrackers.push_back(track);
		}
	}

	trackers = newTrackers;

	if (trackers.size() == 0)
	{
		_id = -1;
	}
}

HipeStatus filter::algos::Tracker::process()
{
	if (_connexData.empty())
	{
		return OK;
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

	

	if (!isSourceFound || image.empty() || !image.getMat().data)
		return OK;

	cv::Mat frame = image.getMat();
	if (frame.channels() == 3)
		cv::cvtColor(frame, frame, CV_RGB2GRAY);

	clearTrackerOutsideFrame(frame);
	//Update if the rect is an old one which has moved
	//OR add it if there is no corresponding tracker
	if (hasIdAndRect(shapes))
	{
		std::vector<std::string> ids;
		std::vector<cv::Rect> new_rects;
		std::vector<cv::Mat> new_rectImgs;
		getIdsAndRect(shapes, ids, new_rects, new_rectImgs);
		
		//For All new rects coming from input and no trackers
		if (trackers.size() == 0)
		{ 
			_id = -1;
			int idxRect = 0;
			for (cv::Rect new_rect : new_rects)
			{
				_id++;
				trackers.push_back(new SingleTracker(_id, new_rectImgs[idxRect], new_rect, ids[idxRect]));
				idxRect++;
			}
		}
		else
		{
			//For All new rects coming from input and at least one tracker exists
			
			int idxRect = 0;
			for (cv::Rect new_rect : new_rects)
			{

				cv::Point new_center = cv::Point(new_rect.x + (new_rect.width / 2),
						new_rect.y + (new_rect.height / 2));
				bool isMatching = false;
				
				for (SingleTracker * track : trackers)
				{
					/*dlib::drectangle drectangle = track.tracker->get_position();*/
					const cv::Rect & prevRect = track->getRect();

					int x_min = std::max(prevRect.x - prevRect.width, 0);
					int y_min = std::max(prevRect.y - prevRect.height, 0);
					//Pass if the new rectangle is in a previous estimate one
					if (new_center.x >= x_min &&
						new_center.x < prevRect.x + 2 * prevRect.width &&
						new_center.y >= y_min &&
						new_center.y < prevRect.y + 2 * prevRect.height)
					{
						isMatching = true;
						track->update(new_rectImgs[idxRect], new_rect);
						
						break;
					}
				} // for (SingleTracker & track : trackers)

				// This is a new Rectangle
				if (! isMatching)
				{
					_id++;

					trackers.push_back(new SingleTracker(_id, new_rectImgs[idxRect], new_rect, ids[idxRect]));
				}
				idxRect++;
			}
		}
	} // if hasIdAndRect(shapes)

	//For all case please update rectangle given by the tracker
	for (SingleTracker * track : trackers)
	{
		auto& history_track = track->history_track;
		double l_confidence = track->update(frame);
		cv::Rect new_pos = track->get_position();
		
		cv::Point center = cv::Point(new_pos.x + (new_pos.width / 2),
			new_pos.y + (new_pos.height / 2));

		bool is_target_inside_frame = isTargetInsideFrame(frame, center);

		if (l_confidence >= confidence && is_target_inside_frame)
		{
			data::ShapeData result;
			if (trail)
			{
				history_track.push_back(center);
				if (history_track.size() > history_points)
				{
					history_track.pop_front();
				}
				std::vector<cv::Point> vec(history_track.begin(), history_track.end());
				result.add(vec);
			}

			
			result.add(cv::Rect(new_pos.x, new_pos.y, new_pos.width, new_pos.height));
			result.IdsArray().push_back(track->getName());

			PUSH_DATA(result);
		}
		else
		{
			LOG(WARNING) << "Lost tracking or confidence for " << track->getName() << std::endl;
			LOG(WARNING) << "Reason " << (l_confidence < confidence ? "Confidence is too low " : (is_target_inside_frame == false) ? "Detection is outFrame" : "something else ??") << std::endl;
		}
	}
	return OK;
}
