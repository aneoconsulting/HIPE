//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#include <filter/algos/streaming/DelegateStreamingFilter.h>
#include <thread>

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
					hipetimeval nextTime;
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
				PUSH_DATA(data::ImageData(cv::Mat::zeros(0, 0, CV_8UC3)));
				return VECTOR_EMPTY;
			}
			data::ImageData image_data = _connexData.pop();

			if (computeFPS() == WAIT_FPS)
			{
				PUSH_DATA(image_data);
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
				PUSH_DATA(data);
			}
			else
			{
				PUSH_DATA(image_data);
			}

			

			return OK;
		}
		
	}
}
