#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/DirPatternData.h>
#include <data/MatcherData.h>
#include <atomic>

#include <boost/timer/timer.hpp>


namespace filter
{
	namespace algos
	{
		
		class FILTER_EXPORT SaveIfNoMatching : public filter::IFilter
		{
			std::atomic<bool> isStart;		//<! [TODO] Is the thread running?
			boost::thread *thr_server;		//<! [TODO] Pointer to the SaveIfNoMatching matching task.
			core::queue::ConcurrentQueue<data::DirPatternData> imagesStack;	//<! [TODO] The queue containing the frames to process.

			core::queue::ConcurrentQueue<data::ImageData> result;	//<! [TODO] The queue containing the results of all the detections to process
			data::MatcherData tosend;									//<! The current frame's matching result to output
			int good_matches;
			int count_frame;										//<! The number of frames already processed 
			int id;										//<! The number of frames already processed 
			cv::Mat saved_matchingImage;
			bool armed;
			boost::timer::cpu_timer timer;


			CONNECTOR(data::MatcherData, data::ImageData);

			REGISTER(SaveIfNoMatching, ()), _connexData(data::INDATA)
			{
				skip_frame = -1;
				id = 0;
				{
					std::stringstream build;
					build << GetCurrentWorkingDir() + PathSeparator() << "patternDir";
					store_directory = build.str();
				}
				thr_server = nullptr;
				armed = false;
				resend_best_during_ms = 5000.;
			}

			REGISTER_P(std::string, store_directory);
			REGISTER_P(double, resend_best_during_ms);
			REGISTER_P(int, skip_frame);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus savePaternPicture(const cv::Mat & picture);

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

		ADD_CLASS(SaveIfNoMatching, store_directory, resend_best_during_ms);
	}
}
