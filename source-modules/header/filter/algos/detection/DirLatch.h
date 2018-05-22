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
#include "Latch.h"
#include "algos/ResultFilter.h"


namespace filter
{
	namespace algos
	{
		/**
		 * \var DirLatch::inlier_threshold
		 * The inliers2_const distance threshold used to match them between images. [TODO]
		 *
		 * \var DirLatch::nn_match_ratio
		 * The nearest neighbor matching ratio. [TODO]
		 *
		 * \var DirLatch::hessianThreshold
		 * The hessian threshold used to find keypoints with the SURF detector. Only features whose hessian is larger than hessianThreshold are retained by the detector. \see cv::xfeatures2d::SURF
		 *
		 * \var DirLatch::skip_frame
		 * The number of frames to skip between each detection.
		 *
		 * \var DirLatch::wait
		 * Should we wait and show the output image?
		 */

		/**
		  *\todo
		  * \brief The DirLatch filter will match similarities into two images.
		  *
		  * It is used to find an object from an image in another one using keypoints by using the DirLatch algorithm.
		  * It awaits a PatternData object in input and will output an image containing the contoured computed matching simimarities.
		  * The filter will work as a separate thread to be able to find objects in videos.
		  * For an alternative: \see Akaze
		 */
		class FILTER_EXPORT DirLatch : public filter::IFilter
		{
			std::atomic<bool> isStart; //<! [TODO] Is the thread running?
			boost::thread* thr_server; //<! [TODO] Pointer to the DirLatch matching task.
			core::queue::ConcurrentQueue<data::DirPatternData> imagesStack; //<! [TODO] The queue containing the frames to process.

			core::queue::ConcurrentQueue<data::MatcherData> result; //<! [TODO] The queue containing the results of all the detections to process
			data::MatcherData tosend; //<! The current frame's matching result to output

			int count_frame; //<! The number of frames already processed 

			std::shared_ptr<filter::algos::Latch> latch;
			std::shared_ptr<filter::algos::ResultFilter> resultLatch;

			CONNECTOR(data::Data, data::MatcherData);

			REGISTER(DirLatch, ()), _connexData(data::INDATA)
			{
				wait_time = 5000;

				skip_frame = -1;
				isStart = false;
				wait = false;
				thr_server = nullptr;
				good_matches = 30;
				inlier_threshold = 4000.f;
				nn_match_ratio = 0.8;
				hessianThreshold = 50;
			}

			HipeStatus detectObject(const data::DirPatternData& pattern);

			REGISTER_P(int, good_matches);

			REGISTER_P(float, inlier_threshold);
			REGISTER_P(float, nn_match_ratio);

			REGISTER_P(int, hessianThreshold);


			REGISTER_P(int, skip_frame);

			REGISTER_P(bool, wait);
			REGISTER_P(int, wait_time);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:

			/**
			 * \brief  Detects patterns in an image. Runs as a separate thread.
			 * Fetch its images from the imagesStack queue then feed the detectObjet method.
			 */
			void startDetectObject();

			HipeStatus process();

			virtual void dispose() override;
		};

		ADD_CLASS(DirLatch, good_matches, inlier_threshold, nn_match_ratio, hessianThreshold, wait, wait_time) ;
	}
}
