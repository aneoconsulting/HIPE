//@HIPE_LICENSE@
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
