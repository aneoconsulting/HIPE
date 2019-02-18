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

#include <filter/algos/detection/Latch.h>

#pragma warning(push, 0)
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		void Latch::startDetectObject()
		{
			Latch* This = this;
			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::PatternData pattern;
					if (!This->imagesStack.trypop_until(pattern, 30))
						continue;
					This->imagesStack.clear();
					try
					{
						data::MatcherData res = This->detectObject(pattern);


						if (This->result.size() != 0)
							This->result.clear();

						This->result.push(res);
					}
					catch (std::exception& e)
					{
						data::MatcherData res;

						This->result.push(res);
					}
				}
			});
		}

		data::MatcherData Latch::detectObject(data::PatternData& patternData)
		{
			double homography_array[9] = {7.6285898e-01, -2.9922929e-01, 2.2567123e+02,
				3.3443473e-01, 1.0143901e+00, -7.6999973e+01,
				3.4663091e-04, -1.4364524e-05, 1.0000000e+00};
			cv::Mat homography = cv::Mat(3, 3, CV_64F, homography_array);


			std::vector<cv::Mat> patterns = patternData.patterns();
			if (patterns.empty())
				throw HipeException("There is no Pattern to find in Latch Algorithm");
			//TODO : manage pultiple crop ....ATM only 1 crop is taken in account
			cv::Mat patternImage = patterns[0];
			cv::Mat requestImage = patternData.imageRequest().getMat();

			std::vector<cv::KeyPoint> kpts1, kpts2;
			cv::Mat desc1, desc2;

			cv::Mat mask;

			cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(hessianThreshold);

			detector->detect(patternImage, kpts1, mask);
			detector->detect(requestImage, kpts2, mask);
			cv::Ptr<cv::xfeatures2d::LATCH> latch = cv::xfeatures2d::LATCH::create();
			latch->compute(patternImage, kpts1, desc1);
			//cv::Ptr<cv::Latch> Latch2 = cv::Latch::create();
			latch->compute(requestImage, kpts2, desc2);

			cv::BFMatcher matcher(cv::NORM_HAMMING);
			std::vector<std::vector<cv::DMatch>> nn_matches;
			matcher.knnMatch(desc1, desc2, nn_matches, 2);

			std::vector<cv::KeyPoint> matched1, matched2, inliers1, inliers2;
			std::vector<cv::DMatch> good_matches;
			for (size_t i = 0; i < nn_matches.size(); i++)
			{
				if (nn_matches[i].size() == 0)
					continue;
				if (kpts1.empty() || kpts2.empty())
					continue;

				cv::DMatch first = nn_matches[i][0];
				float dist1 = nn_matches[i][0].distance;
				float dist2 = nn_matches[i][1].distance;

				if (dist1 < nn_match_ratio * dist2)
				{
					matched1.push_back(kpts1[first.queryIdx]);
					matched2.push_back(kpts2[first.trainIdx]);
				}
			}

			for (unsigned i = 0; i < matched1.size(); i++)
			{
				cv::Mat col = cv::Mat::ones(3, 1, CV_64F);
				col.at<double>(0) = matched1[i].pt.x;
				col.at<double>(1) = matched1[i].pt.y;

				col = homography * col;
				col /= col.at<double>(2);
				double dist = sqrt(pow(col.at<double>(0) - matched2[i].pt.x, 2) +
					pow(col.at<double>(1) - matched2[i].pt.y, 2));

				if (dist < inlier_threshold)
				{
					int new_i = static_cast<int>(inliers1.size());
					inliers1.push_back(matched1[i]);
					inliers2.push_back(matched2[i]);
					good_matches.push_back(cv::DMatch(new_i, new_i, 0));
				}
			}

			data::MatcherData container(patternImage, requestImage, inliers1, inliers2, good_matches);

			return container;
		}

		HipeStatus Latch::process()
		{
			if (!isStart.exchange(true))
			{
				startDetectObject();
			}

			data::PatternData patternData = _connexData.pop();
			if (patternData.crops().getSquareCrop().empty())
			{
				data::MatcherData output;
				PUSH_DATA(output);
				return DATA_EMPTY;
			}

			if (skip_frame <= 0 || count_frame % skip_frame == 0)
			{
				if (imagesStack.size() != 0)
					imagesStack.clear();
				imagesStack.push(patternData);
			}
			count_frame++;

			data::MatcherData md_result;
			cv::Mat res;

			if (patternData.imageRequest().getMat().empty())
			{
				data::MatcherData output;
				PUSH_DATA(output);
			}
			else if (wait == true && result.trypop_until(md_result, wait_time)) // wait 5 sec it's like infinite but allow to kill thread
			{
				tosend = md_result;
				PUSH_DATA(tosend);
			}
			else if (result.trypop_until(md_result, 30)) // wait 30ms no more
			{
				tosend = md_result;
				PUSH_DATA(tosend);
			}

			else if (tosend.requestImage_const().empty())
			{
				data::MatcherData output;
				PUSH_DATA(output);
			}
			else
			{
				md_result = tosend; //Use backup because the algorithme is too late

				PUSH_DATA(tosend);
			}

			return OK;
		}
	}
}
