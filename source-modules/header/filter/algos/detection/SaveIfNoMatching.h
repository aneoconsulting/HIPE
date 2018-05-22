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
#include <data/ImageData.h>
#include <data/DirPatternData.h>
#include <data/MatcherData.h>
#include <atomic>

#include <boost/timer/timer.hpp>


namespace filter
{
	namespace algos
	{
		
		class FILTER_EXPORT SaveIfNoMatching : public filter::IFilter
		{
			std::atomic<bool> isStart;		//<! [TODO] Is the thread running?
			boost::thread *thr_server;		//<! [TODO] Pointer to the SaveIfNoMatching matching task.
			core::queue::ConcurrentQueue<data::DirPatternData> imagesStack;	//<! [TODO] The queue containing the frames to process.

			core::queue::ConcurrentQueue<data::ImageData> result;	//<! [TODO] The queue containing the results of all the detections to process
			data::MatcherData tosend;									//<! The current frame's matching result to output
			int good_matches;
			int count_frame;										//<! The number of frames already processed 
			int id;										//<! The number of frames already processed 
			cv::Mat saved_matchingImage;
			bool armed;
			boost::timer::cpu_timer timer;


			CONNECTOR(data::MatcherData, data::ImageData);

			REGISTER(SaveIfNoMatching, ()), _connexData(data::INDATA)
			{
				skip_frame = -1;
				id = 0;
				{
					std::stringstream build;
					build << GetCurrentWorkingDir() + PathSeparator() << "patternDir";
					store_directory = build.str();
				}
				thr_server = nullptr;
				armed = false;
				resend_best_during_ms = 5000.;
			}

			REGISTER_P(std::string, store_directory);
			REGISTER_P(double, resend_best_during_ms);
			REGISTER_P(int, skip_frame);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus savePaternPicture(const cv::Mat & picture);

			HipeStatus process();

			virtual void dispose()
			{
				isStart = false;

				if (thr_server != nullptr) {
					thr_server->join();
					delete thr_server;
					thr_server = nullptr;
				}
			}
		};

		ADD_CLASS(SaveIfNoMatching, store_directory, resend_best_during_ms);
	}
}
