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

#include <filter/algos/detection/FingerPrint.h>


#pragma warning(push, 0)
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)


using namespace std;
using namespace cv;
using namespace xfeatures2d;

namespace filter
{
	namespace algos
	{

		HipeStatus FingerPrint::process()
		{
			while (!_connexData.empty())
			{
				// Get images
				data::ImageArrayData array(_connexData.pop());
				data::ImageData imagetest(array.Array()[0]);
				data::ImageData imageref(array.Array()[1]);


				// Compute Keypoints and descriptor
				Ptr<Feature2D> orb_descriptor = ORB::create();
				Mat descriptorstest, descriptorsref;
				vector<KeyPoint> keypointstest, keypointsref;
				Ptr<SURF> detector = SURF::create();
				detector->setHessianThreshold(minHessian);

				detector->detectAndCompute(imagetest.getMat(), Mat(), keypointstest, descriptorstest);
				detector->detectAndCompute(imageref.getMat(), Mat(), keypointsref, descriptorsref);

				// Matching descriptor vectors 
				FlannBasedMatcher matcher;
				std::vector< DMatch > matches;
				matcher.match(descriptorstest, descriptorsref, matches);


				//-- Quick calculation of min distances between keypoints
				double min_dist = 1;

				for (auto i = 0; i < descriptorstest.rows; i++)
				{
					double const dist = matches[i].distance;
					if (dist < min_dist) min_dist = dist;

				}
				// Detect "good" match
				std::vector< DMatch > good_matches;
				std::vector< DMatch > match_failed;
				for (auto i = 0; i < descriptorstest.rows; i++)
				{

					if (matches[i].distance <= (std::min)(matchcoeff* min_dist, matchthreshold))
					{
						cerr << matches[i].distance << endl;
						good_matches.push_back(matches[i]);
					}
				}
				
				Mat img_matches;

				// Match succeed if number of good match is > minnumbermatch
				if(good_matches.size()>minnumbermatch)
				{
					putText(imagetest.getMat(), "Match Succeed", Point(15,15), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 255, 0), 2, 8, false);
					// Draw only "good" matches
					drawMatches(imagetest.getMat(), keypointstest, imageref.getMat(), keypointsref,
						good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
						vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

				}
				else
				{
					putText(imagetest.getMat(), "Match Failed", Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 0, 0), 2, 8, false);
					// Draw only "good" matches
					drawMatches(imagetest.getMat(), keypointstest, imageref.getMat(), keypointsref,
						match_failed, img_matches, Scalar::all(-1), Scalar::all(-1),
						vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
				}
			
	
				PUSH_DATA(data::ImageData(img_matches));

			}
			return OK;
		}
	}
}
