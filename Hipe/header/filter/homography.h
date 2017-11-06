#pragma once
#include <filter/tools/RegisterTools.hpp>
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
				std::vector<cv::Point2f> obj;
				std::vector<cv::Point2f> scene;
				for (size_t i = 0; i < md.GoodMatches().size(); i++)
				{
					//-- Get the keypoints from the good matches
					obj.push_back(md.Inliers2()[md.GoodMatches()[i].queryIdx].pt);
					scene.push_back(md.Inliers1()[md.GoodMatches()[i].trainIdx].pt);
				}
				cv::Mat H = findHomography(obj, scene, cv::RANSAC);
				//-- Get the corners from the image_1 ( the object to be "detected" )
				std::vector<cv::Point2f> obj_corners(4);
				obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(md.RequestImage().cols, 0);
				obj_corners[2] = cvPoint(md.RequestImage().cols, md.RequestImage().rows); obj_corners[3] = cvPoint(0, md.RequestImage().rows);
				std::vector<cv::Point2f> scene_corners(4);
				perspectiveTransform(obj_corners, scene_corners, H);
				//-- Draw lines between the corners (the mapped object in the scene - image_2 )
				//TODO REMOVE AFTER TEST
				line(md.GoodMatches(), scene_corners[0] + cv::Point2f(md.RequestImage().cols, 0), scene_corners[1] + cv::Point2f(md.RequestImage().cols, 0), cv::Scalar(0, 255, 0), 4);
				line(md.GoodMatches(), scene_corners[1] + cv::Point2f(md.RequestImage().cols, 0), scene_corners[2] + cv::Point2f(md.RequestImage().cols, 0), cv::Scalar(0, 255, 0), 4);
				line(md.GoodMatches(), scene_corners[2] + cv::Point2f(md.RequestImage().cols, 0), scene_corners[3] + cv::Point2f(md.RequestImage().cols, 0), cv::Scalar(0, 255, 0), 4);
				line(md.GoodMatches(), scene_corners[3] + cv::Point2f(md.RequestImage().cols, 0), scene_corners[0] + cv::Point2f(md.RequestImage().cols, 0), cv::Scalar(0, 255, 0), 4);
				data::ShapeData res;
				std::vector<cv::Point2f> corners;// = std::array<cv::Point2f, 4>(scene_corners[0], scene_corners[1], scene_corners[2], scene_corners[3]);
				corners.push_back(scene_corners[0]);
				corners.push_back(scene_corners[1]);
				corners.push_back(scene_corners[2]);
				corners.push_back(scene_corners[3]);
				res.QuadrilatereArray().push_back(corners);
				_connexData.push(res);
				return OK;
			}

			void dispose()
			{
			}

		};

		ADD_CLASS(Homography, unused);
	}
}
