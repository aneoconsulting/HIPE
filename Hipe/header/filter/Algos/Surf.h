#pragma once
#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/xfeatures2d/nonfree.hpp>


namespace filter
{
	namespace algos
	{
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
