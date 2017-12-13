#pragma once
#include <filter/algos/DelegateStreamingFilter.h>

namespace filter
{
	namespace algos
	{
		HipeStatus DelegateStreamingFilter::computeFPS()
		{
			unsigned long long elapse;
			if (fps_avg == 0)
			{
				nb_frame++;
				if (current_time.tv_sec == 0 && current_time.tv_usec == 0)
				{
					hipe_gettimeofday(&current_time, nullptr);
					return WAIT_FPS;
				}
				else
				{
					struct timeval nextTime;
					hipe_gettimeofday(&nextTime, nullptr);
					elapse = ((nextTime.tv_sec - current_time.tv_sec) * 1000000L
						+ nextTime.tv_usec) - current_time.tv_usec;



					if (elapse > 1000000L)
					{
						double current_fps = static_cast<double>(nb_frame * 1000000L) / elapse;
						nb_frame = 0;
						current_time = nextTime;
						std::cout << " us, FPS : " << current_fps << std::endl;
						fps_avg = current_fps;
						return OK;
					}
					return WAIT_FPS;
				}
			}
			return OK;
		}

		HipeStatus DelegateStreamingFilter::process()
		{
			if (_connexData.empty()) return VECTOR_EMPTY;

			if (computeFPS() == WAIT_FPS)	return OK;


			data::ImageData image_data = _connexData.pop();
			cv::Size size = image_data.getMat().size();
			if (!writer.isOpened()) {
				/*cv::Size size = getImageDimension();*/
				if (cmd != "")
				{
					uri.clear();
					uri << cmd;
					writer.open(cmd, 0, (double)fps_avg, size, true);
				}
				else {
					uri << port;

					writer.open(uri.str(), 0, (double)fps_avg, size, true);
				}
			}
			cv::Mat copy;
			image_data.getMat().copyTo(copy);
			//cv::cvtColor(image_data.getMat(), copy, CV_BGR2YUV_I420);
			writer << copy;

			if (!reader)
			{
				reader.reset(new data::FileVideoInput("udpsrc port=8864 ! application/x-rtp,media=video,clock-rate=90000,encoding-name=H264 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! timeoverlay shaded-background=true deltay=20 ! appsink sync=false", false));
				reader->openFile();
			}
			data::ImageData frame = reader->newFrame();

			_connexData.push(frame);

			return OK;
		}
		
	}
}