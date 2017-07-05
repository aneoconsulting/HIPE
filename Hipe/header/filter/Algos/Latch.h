#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <filter/data/ImageData.h>
#include <filter/data/PatternData.h>
#include <opencv2/xfeatures2d.hpp>
#include <atomic>


namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT Latch : public filter::IFilter
		{
			std::atomic<bool> isStart;
			boost::thread *thr_server;
			core::queue::ConcurrentQueue<data::PatternData> imagesStack;
			class MatchContainer
			{
			public:
				cv::Mat patternImage;
				std::vector<cv::KeyPoint> inliers1;
				
				cv::Mat requestImage;
				std::vector<cv::KeyPoint> inliers2;

				std::vector<cv::DMatch> goodMatches;


			};
			core::queue::ConcurrentQueue<MatchContainer> result;
			MatchContainer tosend;

			int count_frame;

			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(Latch, ()), _connexData(data::INDATA)
			{
				hessianThreshold = 100;
				skip_frame = 2;
				isStart = false;
				
			}

			REGISTER_P(float, inlier_threshold);
			REGISTER_P(float, nn_match_ratio);

			REGISTER_P(int, hessianThreshold);

			REGISTER_P(int, skip_frame);

			


			virtual std::string resultAsString() { return std::string("TODO"); };

		private:
			MatchContainer detectObject(data::PatternData & patternData);

		public:

			void startDetectObject();
			
			HipeStatus process();

			virtual void dispose()
			{
				isStart = false;

				if (thr_server != nullptr) {
					thr_server->join();
					delete thr_server;
					thr_server = nullptr;
				}
			}
		};

		ADD_CLASS(Latch, inlier_threshold, nn_match_ratio, hessianThreshold);
	}
}
