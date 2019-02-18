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
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#pragma warning(push, 0)
#include <opencv2/xfeatures2d/nonfree.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		/**
		 * \var Surf::minHessian
		 * The hessian threshold used to find keypoints with the SURF detector. Only features whose hessian is larger than hessianThreshold are retained by the detector.
		 */

		/**
		 * \brief The Surf filter is used to find keypoints on an image.
		 * 
		 * It uses the SURF algorithm. The keypoints will be drawn on the image.
		 * \see cv::xfeatures2d::SURF
		 */
		class Surf : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(Surf, ()), _connexData(data::INOUT)
			{

			}

			REGISTER_P(int, minHessian);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				while (!_connexData.empty()) // While i've parent data
				{
					data::ImageArrayData images = _connexData.pop();
					if (images.getType() == data::PATTERN)
					{
						throw HipeException("The resize object cant resize PatternData. Please Develop ResizePatterData");
					}

					//Resize all images coming from the same parent
					for (auto &myImage : images.Array())
					{
						myImage = keypoints_SURF(myImage, minHessian);
					}
				}
				return OK;
			}


			cv::Mat keypoints_SURF(cv::Mat img, int minHessian) const
			{
				// Create SURF Detector

				cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);

				// Compute keypoint
				std::vector<cv::KeyPoint> kp;
				cv::Mat desc;
				detector->detectAndCompute(img, cv::Mat(), kp, desc);
				std::cout << "SURF :: " << kp.size() << " Keypoints " << std::endl;

				//-- Draw keypoints
				cv::Mat img_kp_SURF;
				drawKeypoints(img, kp, img_kp_SURF, cv::Scalar(255, 0, 0), cv::DrawMatchesFlags::DEFAULT);
				return img_kp_SURF;
				//algos::ShowImage(img_kp_SURF);
				//imshow("SURF keypoints", img_kp);
			}
		};

		ADD_CLASS(Surf, minHessian);
	}






	//bool findObjectSURF(Mat objectMat, Mat sceneMat, int hessianValue)
	//{
	//	bool objectFound = false;
	//	float nndrRatio = 0.7f;
	//	//vector of keypoints   
	//	vector< cv::KeyPoint > keypointsO;
	//	vector< cv::KeyPoint > keypointsS;
	//
	//	Mat descriptors_object, descriptors_scene;
	//
	//	//-- Step 1: Extract keypoints
	//	SurfFeatureDetector surf(hessianValue);
	//	surf.detect(sceneMat, keypointsS);
	//	if (keypointsS.size() < 7) return false; //Not enough keypoints, object not found
	//	surf.detect(objectMat, keypointsO);
	//	if (keypointsO.size() < 7) return false; //Not enough keypoints, object not found
	//
	//											 //-- Step 2: Calculate descriptors (feature vectors)
	//	SurfDescriptorExtractor extractor;
	//	extractor.compute(sceneMat, keypointsS, descriptors_scene);
	//	extractor.compute(objectMat, keypointso, descriptors_object);
	//
	//	//-- Step 3: Matching descriptor vectors using FLANN matcher
	//	FlannBasedMatcher matcher;
	//	descriptors_scene.size(), keypointsO.size(), keypointsS.size());
	//	std::vector< vector< DMatch >  > matches;
	//	matcher.knnMatch(descriptors_object, descriptors_scene, matches, 2);
	//	vector< DMatch > good_matches;
	//	good_matches.reserve(matches.size());
	//
	//	for (size_t i = 0; i < matches.size(); ++i)
	//	{
	//		if (matches[i].size() < 2)
	//			continue;
	//
	//		const DMatch &m1 = matches[i][0];
	//		const DMatch &m2 = matches[i][1];
	//
	//		if (m1.distance <= nndrRatio * m2.distance)
	//			good_matches.push_back(m1);
	//	}
	//
	//
	//
	//	if ((good_matches.size() >= 7))
	//	{
	//
	//		cout << "OBJECT FOUND!" << endl;
	//
	//		std::vector< Point2f > obj;
	//		std::vector< Point2f > scene;
	//
	//		for (unsigned int i = 0; i < good_matches.size(); i++)
	//		{
	//			//-- Get the keypoints from the good matches
	//			obj.push_back(keypointsO[good_matches[i].queryIdx].pt);
	//			scene.push_back(keypointsS[good_matches[i].trainIdx].pt);
	//		}
	//
	//		Mat H = findHomography(obj, scene, CV_RANSAC);
	//
	//
	//
	//		//-- Get the corners from the image_1 ( the object to be "detected" )
	//		std::vector< Point2f > obj_corners(4);
	//		obj_corners[0] = cvPoint(0, 0); obj_corners[1] = cvPoint(objectMat.cols, 0);
	//		obj_corners[2] = cvPoint(objectMat.cols, objectMat.rows); obj_corners[3] = cvPoint(0, objectMat.rows);
	//		std::vector< Point2f > scene_corners(4);
	//
	//		perspectiveTransform(obj_corners, scene_corners, H);
	//
	//
	//		//-- Draw lines between the corners (the mapped object in the scene - image_2 ) 
	//		line(outImg, scene_corners[0], scene_corners[1], color, 2); //TOP line
	//		line(outImg, scene_corners[1], scene_corners[2], color, 2);
	//		line(outImg, scene_corners[2], scene_corners[3], color, 2);
	//		line(outImg, scene_corners[3], scene_corners[0], color, 2);
	//		objectFound = true;
	//	}
	//	else {
	//		cout << "OBJECT NOT FOUND!" << endl;
	//	}
	//
	//
	//	cout << "Matches found: " << matches.size() << endl;
	//	cout << "Good matches found: " << good_matches.size() << endl;
	//
	//	return objectFound;
	//}
}
