//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#pragma warning(push, 0)
#include <boost/thread.hpp>
#pragma warning(pop)

#include <atomic>

#include <data/ImageData.h>
#include <data/DlibDetectorData.h>

#include <data/HogTrainer/HogTrainer.h>


namespace filter
{
	namespace algos
	{
		class HOGLiveTrainer : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::DlibDetectorData);

			REGISTER(HOGLiveTrainer, ()), _connexData(data::WayData::INDATA)
			{
				_isThreadRunning = true;
				_pFilterThread = nullptr;
				_countProcessedFrames = 0;

				skip_frames = 4;
				
				training_images_frequency = 2;
				configuration_interval = 5;
				viewport_size_x = 150;
				viewport_size_y = 150;
				viewport_offset_x = 0;
				viewport_offset_y = 175;
				fhog_epsilon = 0.01;
				training_threads = 2;

				startFilterThread();
			}

			REGISTER_P(int, skip_frames);
			REGISTER_P(int, training_images_frequency);
			REGISTER_P(double, configuration_interval);
			REGISTER_P(int, viewport_size_x);
			REGISTER_P(int, viewport_size_y);
			REGISTER_P(int, viewport_offset_x);
			REGISTER_P(int, viewport_offset_y);
			REGISTER_P(double, fhog_epsilon);
			REGISTER_P(int, training_threads);

		private:
			std::atomic<bool> _isThreadRunning;								//<! boolean to know if the main thread of the filter is already running
			boost::thread* _pFilterThread;									//<! pointer to the filter's asynchronous thread

			core::queue::ConcurrentQueue<data::ImageData> _inputDataStack;			//<! [TODO] The queue containing the frames to process.
			core::queue::ConcurrentQueue<data::DlibDetectorData> _outputDataStack;	//<! [TODO] The queue containing the results of all the detections to process

			data::DlibDetectorData _outputData;

			data::hog_trainer::HogTrainer<data::hog_trainer::image_scanner_type> _ht;

			size_t _countProcessedFrames;

		public:
			HipeStatus process() override;
			void dispose() override;

		private:
			void startFilterThread();

			void pushInputFrame(const data::ImageData& frame);
			void skipFrames();
			data::DlibDetectorData popOutputData();
		};
		ADD_CLASS(HOGLiveTrainer, skip_frames, training_images_frequency, configuration_interval, viewport_size_x, viewport_size_y, viewport_offset_x, viewport_offset_y, fhog_epsilon, training_threads);
	}
}
