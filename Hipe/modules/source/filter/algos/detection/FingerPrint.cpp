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
			
	
				_connexData.push(data::ImageData(img_matches));

			}
			return OK;
		}
	}
}
