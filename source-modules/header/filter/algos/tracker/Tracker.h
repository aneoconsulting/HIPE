//@HIPE_LICENSE@
#pragma once
#include <deque>
#pragma warning(push, 0)
#include <opencv2/core/mat.hpp>
#if defined(USE_DLIB)
//issue order of header for vector keyword call it before 
#if defined(__ALTIVEC__)
		#include <dlib/simd.h>
#endif

#include <dlib/image_transforms/fhog.h>
#include <dlib/image_processing/correlation_tracker.h>
#endif
#pragma warning(pop)

#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>
#include <data/ShapeData.h>


namespace filter
{
	namespace algos
	{
		class SingleTracker
		{
		public:
			int targetId;
			cv::Rect _rect;

			cv::Rect getRect() const
			{
				return _rect;
			}

			void setRect(const cv::Rect& rect2_i)
			{
				_rect = rect2_i;
			}

			cv::Rect get_position();

			std::string _name;

			std::string getName() const
			{
				return _name;
			}

			SingleTracker()
			{
				tracker = nullptr;
			}

			SingleTracker(int id, cv::Mat& img, const cv::Rect rect, std::string& name);

			SingleTracker(const SingleTracker& left);

			~SingleTracker();

			double update_noscale(const cv::Mat& mat);
			double update_noscale(const cv::Mat& mat, cv::Rect& rect);
			dlib::correlation_tracker* tracker; // Correlation tracker

			std::deque<cv::Point> history_track;
		};

		class FILTER_EXPORT Tracker : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ShapeData);

			REGISTER(Tracker, ()), _connexData(data::INDATA)
			{
				_init = false;
				_id = -1;
				history_points = 20;
				confidence = 30;
				trail = false;
			}

			int _id;
			std::vector<SingleTracker *> trackers;

			REGISTER_P(double, confidence);
			REGISTER_P(int, history_points);
			REGISTER_P(bool, trail);

			std::atomic<bool> _init;

			void clearTrackerOutsideFrame(const cv::Mat& frame);

			HipeStatus process() override;
		};

		ADD_CLASS(Tracker, confidence, history_points) ;
	}
}
