#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <boost/thread.hpp>
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
			REGISTER(HOGLiveTrainer, ()), _connexData(data::INDATA)
			{
				_isThreadRunning = true;
				_pFilterThread = nullptr;
				_countProcessedFrames = 0;

				skip_frames = 0;

				startFilterThread();
			}
			REGISTER_P(int, skip_frames);

		private:
			std::atomic<bool> _isThreadRunning;								//<! boolean to know if the main thread of the filter is already running
			boost::thread* _pFilterThread;									//<! pointer to the filter's asynchronous thread

			core::queue::ConcurrentQueue<data::ImageData> _inputDataStack;			//<! [TODO] The queue containing the frames to process.
			core::queue::ConcurrentQueue<data::DlibDetectorData> _outputDataStack;	//<! [TODO] The queue containing the results of all the detections to process

			::data::hog_trainer::HogTrainer<::data::hog_trainer::image_scanner_type> _ht;

			size_t _countProcessedFrames;

		public:
			HipeStatus process() override;
			void dispose() override;

		private:
			void startFilterThread();

			inline void pushInputFrame(const data::ImageData& frame);
			inline void skipFrames();
			inline data::DlibDetectorData popOutputData();
		};
		ADD_CLASS(HOGLiveTrainer, skip_frames);
	}
}
