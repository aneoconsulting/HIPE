#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>
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

			}
			REGISTER_P(int, unused);

			~Homography()
			{
			}

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				data::MatcherData md = _connexData.pop();

				if (md.goodMatches().empty())
				{
					_connexData.push(data::ShapeData());
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

				// Note: Some errors occured BEFORE fixing the code, so the try/catch block may not be mandatory, but mind the filter should not kill the whole process when playing a video. 
				try
				{
					cv::Mat H = findHomography(obj, scene, cv::RANSAC);


					//-- Get the corners from the image_1 ( the object to be "detected" )
					std::vector<cv::Point2f> obj_corners(4);
					obj_corners[0] = cvPoint(0, 0);
					obj_corners[1] = cvPoint(md.patternImage().cols, 0);

					obj_corners[2] = cvPoint(md.patternImage().cols, md.patternImage().rows);
					obj_corners[3] = cvPoint(0, md.patternImage().rows);

					std::vector<cv::Point2f> scene_corners(4);
					perspectiveTransform(obj_corners, scene_corners, H);

					data::ShapeData res;
					std::vector<cv::Point2f> corners;
					corners.push_back(scene_corners[0]);
					corners.push_back(scene_corners[1]);
					corners.push_back(scene_corners[2]);
					corners.push_back(scene_corners[3]);

					res.QuadrilatereArray().push_back(corners);

					_connexData.push(res);
				}
				catch (const std::exception& e)
				{
					std::stringstream errorMessage;
					errorMessage << "Error in Homography filter: " << e.what();
					//throw HipeException(errorMessage.str());
					std::cout << errorMessage.str();
					_connexData.push(data::ShapeData());
				}

				return OK;
			}

			void dispose()
			{
			}

		};

		ADD_CLASS(Homography, unused);
	}
}
