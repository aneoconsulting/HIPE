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

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#pragma warning(pop)

#include <data/ShapeData.h>
#include <data/MatcherData.h>

namespace filter
{
	namespace algos
	{

		class Homography : public IFilter
		{

			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::MatcherData, data::ShapeData);

			REGISTER(Homography, ()), _connexData(data::INDATA)
			{
				draw_matches = true;
				_debug = false;
			}
			REGISTER_P(bool, draw_matches);
			REGISTER_P(bool, _debug);

			~Homography()
			{
			}

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				data::MatcherData md = _connexData.pop();

				// No keypoints found means no homography matrix
				if (md.goodMatches().empty())
				{
					PUSH_DATA(data::ShapeData());
					return OK;
				}

				std::vector<cv::Point2f> obj;
				std::vector<cv::Point2f> scene;

				for (size_t i = 0; i < md.goodMatches().size(); i++)
				{
					//-- Get the keypoints from the good matches
					obj.push_back(md.inliers1()[md.goodMatches()[i].queryIdx].pt);
					scene.push_back(md.inliers2()[md.goodMatches()[i].queryIdx].pt);
				}

				// See also this article: https://docs.opencv.org/3.1.0/d7/dff/tutorial_feature_homography.html
				// The try / catch block may not be mandatory, but some errors occured with cv::findHomography and cv::perspectiveTransform.Regardless, mind the filter should not kill the whole process when playing a video.
				try
				{
					cv::Mat H = findHomography(obj, scene, cv::RANSAC);

					// not enough keypoints means no homography matrix
					if (!H.data)
					{
						if (_debug) std::cout << "Warning in Homography filter: Couldn't compute homography matrix. Not enough keypoints (" << obj.size() << ")." << std::endl;
						PUSH_DATA(data::ShapeData());
						return OK;;
					}


					//-- Get the corners from the image_1 ( the object to be "detected" )
					std::vector<cv::Point2f> obj_corners(4);
					obj_corners[0] = cv::Point2f(0, 0);
					obj_corners[1] = cv::Point2f(md.patternImage().cols, 0);

					obj_corners[2] = cv::Point2f(md.patternImage().cols, md.patternImage().rows);
					obj_corners[3] = cv::Point2f(0, md.patternImage().rows);

					std::vector<cv::Point2f> scene_corners(4);
					perspectiveTransform(obj_corners, scene_corners, H);

					data::ShapeData res;
					std::vector<cv::Point2f> corners;
					corners.push_back(scene_corners[0]);
					corners.push_back(scene_corners[1]);
					corners.push_back(scene_corners[2]);
					corners.push_back(scene_corners[3]);

					res.QuadrilatereArray().push_back(corners);

					// If the user want to, put also the keypoints in the ShapeData object
					if (draw_matches)
					{
						std::vector<cv::Point2f> transformedPoints;
						cv::perspectiveTransform(obj, transformedPoints, H);
						res.PointsArray() = transformedPoints;
					}

					PUSH_DATA(res);
				}
				catch (const std::exception& e)
				{
					if (_debug)
					{
						std::stringstream errorMessage;
						errorMessage << "Error in Homography filter: " << e.what();
						//throw HipeException(errorMessage.str());
						std::cout << errorMessage.str();
					}
					PUSH_DATA(data::ShapeData());
				}

				return OK;
			}

			void dispose()
			{
			}

		};

		ADD_CLASS(Homography, draw_matches, _debug);
	}
}
