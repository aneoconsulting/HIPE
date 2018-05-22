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
