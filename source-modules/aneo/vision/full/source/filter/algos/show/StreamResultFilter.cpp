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

#include <filter/algos/StreamResultFilter.h>



namespace filter
{
	namespace algos
	{
		std::string buildGstreamUri(std::string ip_dest, int udp_port)
		{
			std::stringstream buildUri;

			//appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtph264pay config-interval=10 pt=96 mtu=1400 ! udpsink host=192.168.1.19 port=8864 sync=false async=false
			buildUri << "appsrc ! videoconvert ! x264enc tune=zerolatency bitrate=500 speed-preset=superfast ! rtph264pay config-interval=10 pt=96 mtu=1400";

			buildUri << " ! udpsink host=";
			buildUri << ip_dest;

			buildUri << " port=";
			buildUri << udp_port;

			buildUri << " sync=false async=false";
			return buildUri.str();
		}

		HipeStatus StreamResultFilter::computeFPS()
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

		cv::Size StreamResultFilter::getImageDimension()
		{
			cv::Mat first = _connexData.get().getMat(); // this call doesn't pop the data from the queue

			return first.size();
		}

		HipeStatus StreamResultFilter::process()
		{
			if (_connexData.empty()) return VECTOR_EMPTY;

			//if (computeFPS() == WAIT_FPS)	return OK;
			fps_avg = 25;
			
			data::ImageData image_data = _connexData.pop();
			cv::Size size = image_data.getMat().size();
			if (!writer.isOpened()) {
				/*cv::Size size = getImageDimension();*/
				if (ip_dest != "")
				{
					uri.clear();

					std::string cs = buildGstreamUri(ip_dest, port);
					std::cout << "RTP connextion to " << std::endl << cs << std::endl;
					writer.open(cs, 0, (double)fps_avg, size, true);
				}
				else {
					uri << port;
					std::cout << "RTP connextion to " << std::endl << uri.str() << std::endl;
					writer.open(uri.str(), 0, (double)fps_avg, size, true);
				}
			}
			cv::Mat copy;
			if (!image_data.getMat().empty())
			{
				image_data.getMat().copyTo(copy);
				//cv::cvtColor(image_data.getMat(), copy, CV_BGR2YUV_I420);
				writer << copy;
			}

			return OK;
		}
	}
}
