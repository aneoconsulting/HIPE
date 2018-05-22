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
#include <deque>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>
#include <data/ShapeData.h>

#pragma warning(push, 0)
#include <opencv2/core/mat.hpp>
#include <dlib/image_transforms/fhog.h>
#include <dlib/image_processing/correlation_tracker.h>
#pragma warning(pop)



namespace filter
{
	namespace algos
	{
		struct SingleTracker
		{
			int targetId;

			SingleTracker(int id, cv::Mat& img, const cv::Rect rect);
			
			double update_noscale(const cv::Mat& mat);
			dlib::correlation_tracker tracker;  // Correlation tracker
			
			std::deque<cv::Point> history_track;

		};

		class FILTER_EXPORT Tracker : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ShapeData);

			REGISTER(Tracker, ()), _connexData(data::INDATA)
			{
				_init = false;
				_id = -1;
				history_points = 20;
				confidence = 30;
			}
			
			int _id;
			std::vector<SingleTracker> trackers;

			REGISTER_P(double, confidence);
			REGISTER_P(int, history_points);

			std::atomic<bool> _init;

			HipeStatus process() override;


		};

		ADD_CLASS(Tracker, confidence, history_points);
	}
}
