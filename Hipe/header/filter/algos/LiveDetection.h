#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <boost/thread.hpp>
#include <atomic>

#include <data/ImageData.h>


namespace filter
{
	namespace algos
	{
		class LiveDetection : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::Data);
			REGISTER(LiveDetection, ()), _connexData(data::INDATA)
			{
				_isThreadRunning = true;
				_pFilterThread = nullptr;

				startFilterThread();
			}
			REGISTER_P(char, unused);

		private:
			std::atomic<bool> _isThreadRunning;								//<! boolean to know if the main thread of the filter is already running
			boost::thread* _pFilterThread;									//<! pointer to the filter's asynchronous thread

			core::queue::ConcurrentQueue<data::Data> _inputDataStack;	//<! [TODO] The queue containing the frames to process.
			core::queue::ConcurrentQueue<data::Data> _outputDataStack;	//<! [TODO] The queue containing the results of all the detections to process
			data::Data _dataToSend;									//<! The current frame's matching result to output

		public:
			HipeStatus process() override;
			void dispose() override;

		private:
			void startFilterThread();

			data::Data processLiveTraining(const data::Data& data);
		};
		ADD_CLASS(LiveDetection, unused);
	}
}
