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

#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <core/misc.h>
#include <core/queue/ConcurrentQueue.h>

#include <corefilter/tools/filterMacros.h>
#include <data/ImageData.h>
#include <string>

#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		/**
		 * \var StreamResultFilter::port
		 * The port on which the target device is listening.
		 *
		 * \var StreamResultFilter::cmd
		 * The command line containing all the streaming parameters.
		 */

		/**
		 * \todo
		 * \brief The StreamResultFilter filter is used to stream the result of a graph to a distant target.
		 */
		class FILTER_EXPORT StreamResultFilter : public IFilter
		{
			//data::ConnexOutput<data::ImageData> connData;
			CONNECTOR_OUT(data::ImageData);

			core::queue::ConcurrentQueue<data::ImageData> queue;	//<! [TODO] unused?
			hipetimeval current_time;									//<! The current time.
			int fps_avg;	//<! [TODO] The average FPS at which the video is played.
			int nb_frame;	//<! [TODO]


			// second part of sender pipeline
			std::stringstream uri;	//<! The uri of the device on which the video will be streamed.
			cv::VideoWriter writer;	//<! [TODO]

			/* int setenv(const char *name, const char *value, int overwrite) */
			/* { */
			/* 	int errcode = 0; */
			/* 	if (!overwrite) { */
			/* 		size_t envsize = 0; */
			/* 		errcode = getenv_s(&envsize, NULL, 0, name); */
			/* 		if (errcode || envsize) return errcode; */
			/* 	} */
			/* 	return _putenv_s(name, value); */
			/* } */

			REGISTER(StreamResultFilter, ())
			{
				port = 8864;
				current_time.tv_sec = 0;
				current_time.tv_usec = 0;
				fps_avg = 0;
				nb_frame = 0;

				//setenv("GST_DEBUG", "cat:level...", 1);
				uri << "appsrc ! videoconvert ! x264enc ! rtph264pay config-interval=10 pt=96 ! udpsink host=192.168.1.255 auto-multicast=true port=";

			}


			REGISTER_P(int, port);

			REGISTER_P(std::string, cmd);

			~StreamResultFilter()
			{

			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

			void StreamVideo()
			{

			}

		public:
			/**
			 * \brief [TODO] Computes the average fps at which the video is played.
			 * \return [TODO]
			 */
			HipeStatus computeFPS();

			/**
			 * \brief Get the dimensions of the video.
			 * \return Returns the dimensions of the video as a cv::Size object.
			 */
			cv::Size getImageDimension();

			HipeStatus process();

			/*virtual void cleanUp()
			{


			}*/

			void dispose()
			{
				current_time.tv_sec = current_time.tv_usec = 0;

				nb_frame = 0;

				if(writer.isOpened())
				{
					writer.release();
				}
			}
		};

		ADD_CLASS(StreamResultFilter, cmd, port);

	}
}
