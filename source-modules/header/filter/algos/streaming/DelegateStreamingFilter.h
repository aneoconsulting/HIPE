//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <core/misc.h>
#include <data/ImageData.h>
#include <data/FileVideoInput.h>

namespace filter
{
	namespace algos
	{

		class DelegateStreamingFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			core::queue::ConcurrentQueue<data::ImageData> queue;	//<! [TODO] unused?
			hipetimeval current_time;									//<! The current time.
			int fps_avg;	//<! [TODO] The average FPS at which the video is played.
			int nb_frame;	//<! [TODO]
			std::atomic<bool> isStart;
			boost::thread *readerTask;		//<! [TODO] Pointer to the face detection task.

			// second part of sender pipeline
			std::stringstream uri;	//<! The uri of the device on which the video will be streamed.
			cv::VideoWriter writer;	//<! [TODO]
			std::shared_ptr<data::FileVideoInput> reader;

			REGISTER(DelegateStreamingFilter, ()), _connexData(data::INDATA)
			{
				port = 8864;
				address = "192.168.99.99";
				current_time.tv_sec = 0;
				current_time.tv_usec = 0;
				fps_avg = 0;
				nb_frame = 0;
				isStart = false;
				//setenv("GST_DEBUG", "cat:level...", 1);
				uri << "appsrc ! videoconvert ! openh264enc ! video/x-h264, stream-format=(string)byte-stream ! h264parse ! rtph264pay mtu=1400 ! udpsink host=@address port=@port sync=false async=false";
				readerTask = nullptr;
			}

			REGISTER_P(std::string, cmd);
			REGISTER_P(std::string, address);

			REGISTER_P(int, port);

		public:
			HipeStatus computeFPS();

			HipeStatus process() override;

			virtual void dispose()
			{
				isStart = false;

				if (readerTask != nullptr) {
					readerTask->join();
					delete readerTask;
					readerTask = nullptr;
				}
			}

			void startStreamReader();


		};

		ADD_CLASS(DelegateStreamingFilter, port, cmd, address);
	}
}