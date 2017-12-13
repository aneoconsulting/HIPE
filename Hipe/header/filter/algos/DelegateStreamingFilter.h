#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include "data/ImageData.h"
#include <data/FileVideoInput.h>

namespace filter
{
	namespace algos
	{

		class DelegateStreamingFilter : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			core::queue::ConcurrentQueue<data::ImageData> queue;	//<! [TODO] unused?
			struct timeval current_time;									//<! The current time.
			int fps_avg;	//<! [TODO] The average FPS at which the video is played.
			int nb_frame;	//<! [TODO]

			// second part of sender pipeline
			std::stringstream uri;	//<! The uri of the device on which the video will be streamed.
			cv::VideoWriter writer;	//<! [TODO]
			std::shared_ptr<data::FileVideoInput> reader;

			REGISTER(DelegateStreamingFilter, ()), _connexData(data::INDATA)
			{
				port = 8864;
				current_time.tv_sec = 0;
				current_time.tv_usec = 0;
				fps_avg = 0;
				nb_frame = 0;

				//setenv("GST_DEBUG", "cat:level...", 1);
				uri << "appsrc ! videoconvert ! x264enc ! rtph264pay config-interval=10 pt=96 ! udpsink host=192.168.1.255 auto-multicast=true port=";

			}

			REGISTER_P(std::string, cmd);
			REGISTER_P(std::string, address);

			REGISTER_P(int, port);

		public:
			HipeStatus computeFPS();

			HipeStatus process() override;

		};

		ADD_CLASS(DelegateStreamingFilter, port, cmd, address);
	}
}