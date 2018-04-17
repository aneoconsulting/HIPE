#pragma once
#include <deque>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>
#include <data/ShapeData.h>

#pragma warning(push, 0)
#include <opencv2/core/mat.hpp>
#include <dlib/image_transforms/fhog.h>
#include <dlib/image_processing/correlation_tracker.h>
#pragma warning(pop)



namespace filter
{
	namespace algos
	{
		struct SingleTracker
		{
			int targetId;

			SingleTracker(int id, cv::Mat& img, const cv::Rect rect);
			
			double update_noscale(const cv::Mat& mat);
			dlib::correlation_tracker tracker;  // Correlation tracker
			
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
			}
			
			int _id;
			std::vector<SingleTracker> trackers;

			REGISTER_P(double, confidence);
			REGISTER_P(int, history_points);

			std::atomic<bool> _init;

			HipeStatus process() override;


		};

		ADD_CLASS(Tracker, confidence, history_points);
	}
}
