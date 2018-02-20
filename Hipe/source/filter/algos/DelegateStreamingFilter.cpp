#include <filter/algos/DelegateStreamingFilter.h>
#include <thread>
#include <boost/thread/thread.hpp>
#include <chrono>

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
		void DelegateStreamingFilter::startStreamReader()
		{
			DelegateStreamingFilter* This = this;

			readerTask = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::ImageData image;
					if (!This->reader)
					{
						This->reader.reset(new data::FileVideoInput("udpsrc port=8865 ! application/x-rtp,media=video,clock-rate=90000,encoding-name=H264 ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! timeoverlay shaded-background=true deltay=20 ! appsink sync=false", false));
						std::this_thread::sleep_for(std::chrono::seconds(40));
						This->reader->openFile();
					}
					data::ImageData frame = This->reader->newFrame();


					/*if (This->queue.size() != 0)
						This->queue.clear();*/

					This->queue.push(data::ImageData(frame));
				}
			});
		}

		HipeStatus DelegateStreamingFilter::process()
		{
			if (_connexData.empty())
			{
				_connexData.push(data::ImageData(cv::Mat::zeros(0, 0, CV_8UC3)));
				return VECTOR_EMPTY;
			}
			data::ImageData image_data = _connexData.pop();

			if (computeFPS() == WAIT_FPS)
			{
				_connexData.push(image_data);
				return OK;
			}


			
			cv::Size size = image_data.getMat().size();
			if (!writer.isOpened()) 
			{
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

			if (!isStart.exchange(true))
			{
				startStreamReader();
			}

			data:data::ImageData data;

			if (queue.trypop_until(data, 30))
			{
				_connexData.push(data);
			}
			else
			{
				_connexData.push(image_data);
			}

			

			return OK;
		}
		
	}
}
