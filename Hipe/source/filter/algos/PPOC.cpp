#include <filter\Algos\PPOC.h>

HipeStatus filter::algos::PPOC::process()
{
	// TODO: Send multiple data from JSON
	data::ImageArrayData data = _connexData.pop();
	//
	//// Empty connector case
	//if (data.empty())
	//{
	//	throw HipeException("No Data found in connector");
	//}

	//// Not enough data case
	//if (data.Array_const().size() < 3)
	//{
	//	throw HipeException("Not enough data in connector...\nExpected to find at least (in order) the reference image, the reference image's background, then some parts to find");
	//}
	//
	//cv::Mat referenceImage = data.Array().at(0);
	//cv::Mat background = data.Array().at(1);
	//
	//std::vector<cv::Mat> queries;
	//
	//// Get query images
	//for (std::vector<cv::Mat>::const_iterator it = data.Array_const().begin() + 2; it != data.Array_const().end(); ++it)
	//{
	//	queries.push_back(it->clone());
	//}


	cv::Mat referenceImage, background;
	std::vector<cv::Mat> queries;
	const int downscaleRatio = 3;
	referenceImage = cv::imread("C:\\workspace\\gitlab_aneo\\Hipe\\Data\\Images\\OK.JPG");
	referenceImage = filter::algos::IDPlate::downscaleImage(referenceImage, downscaleRatio);

	background = cv::imread("C:\\workspace\\gitlab_aneo\\Hipe\\Data\\Images\\back.jpg");
	background = filter::algos::IDPlate::downscaleImage(background, downscaleRatio);

	queries.push_back(cv::imread("C:\\workspace\\gitlab_aneo\\Hipe\\Data\\Images\\template\\vp.png"));
	queries[0] = filter::algos::IDPlate::downscaleImage(queries[0], downscaleRatio);

	// Debug
	if (_debug)
	{
		filter::algos::IDPlate::showImage(referenceImage);
		filter::algos::IDPlate::showImage(background);
		filter::algos::IDPlate::showImage(queries.front());
	}

	// Preprocess background
	cv::Mat mask;
	preprocessBackgroundImage(referenceImage, background, mask);

	// Debug
	if (_debug)
	{
		filter::algos::IDPlate::showImage(referenceImage);
		filter::algos::IDPlate::showImage(background);
		filter::algos::IDPlate::showImage(mask);
	}

	// Apply background mask
	cv::Mat masked;
	referenceImage.copyTo(masked, mask);
	referenceImage = masked;

	// Debug
	if (_debug)
	{
		filter::algos::IDPlate::showImage(referenceImage);
	}

	// Legacy code
	//for (MatIterator_<Vec3b> it = extractedObject.begin<Vec3b>(), itQuery = extractedQueryObject.begin<Vec3b>();
	//	it != extractedObject.end<Vec3b>(); it++, itQuery++)
	//{
	//	if ((*it_mask) == (char)0)
	//	{
	//		(*it) = 0;
	//		//(*itQuery) = 0;	// TM commented line
	//	}
	//	it_mask++;
	//}

	// Find reference's descriptors
	// Find query objects
	for (auto & query : queries)
	{
		findObject(referenceImage, query, mask);
	}

	_connexData.push(filter::data::ImageData(referenceImage));
	_connexData.push(filter::data::ImageData(background));
	for (auto & query : queries)
		_connexData.push(filter::data::ImageData(query));

	return OK;
}

cv::Mat filter::algos::PPOC::preprocessBackgroundImage(const cv::Mat & image, const cv::Mat & background, cv::Mat & out_mask)
{
	// Threshold for canny
	const int thresh = 85;

	cv::Mat workImage = image.clone();

	bool matchPerspective = true;
	cv::Mat straigtenedImage = matchImageToBackground(workImage, background);

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(straigtenedImage);

	if (matchPerspective) workImage = straigtenedImage;

	// Create Background Subtractor objects (MOG2 approach)
	cv::Ptr<cv::BackgroundSubtractor> pMOG2;
	if (pMOG2.empty())
		pMOG2 = cv::createBackgroundSubtractorMOG2(2, 800, false); //(1, 900, false);
	pMOG2->apply(background, out_mask);
	pMOG2->apply(workImage, out_mask);
	if (_debug)	filter::algos::IDPlate::showImage(out_mask);

	// TM: Fill shape
	cv::Size kernelSize(31, 31);
	cv::Mat morphKernel = cv::getStructuringElement(cv::MorphShapes::MORPH_ELLIPSE, kernelSize);
	cv::morphologyEx(out_mask, out_mask, cv::MorphTypes::MORPH_CLOSE, morphKernel);
	if (_debug)	filter::algos::IDPlate::showImage(out_mask);

	// Smooth the mask to reduce noise in image
	cv::GaussianBlur(out_mask, out_mask, cv::Size(9, 9), 3.5, 3.5);
	if (_debug)	filter::algos::IDPlate::showImage(out_mask);

	//// Threshold mask to saturate at black and white values
	//cv::threshold(out_mask, out_mask, 10, 255, cv::THRESH_BINARY);
	//if (_debug)	filter::algos::IDPlate::showImage(out_mask);

	// Retrieve longest contour
	// VS : [OPT] We only need to have a look at the outtermost contours (RETR_EXTERNAL)
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(out_mask, contours, hierarchy, cv::RETR_EXTERNAL /*CV_FILLED*/, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	if (_debug)	filter::algos::IDPlate::showImage(out_mask);

	int distance = 0;
	size_t index = 0;
	for (size_t i = 0; i < contours.size(); i++)
	{
		if ((contours[i]).size() > distance)
		{
			distance = (contours[i]).size();
			index = i;
		}
	}

	//erode(mask, mask, Mat(), Point(-1, -1), 3);
	//dilate(mask, mask, Mat(), Point(-1, -1), 3);

	// Draw contours
	cv::Scalar color = cv::Scalar(255);
	out_mask = cv::Mat::zeros(out_mask.size(), CV_8UC1);
	cv::drawContours(out_mask, contours, (int)index, color, 2, 8, hierarchy, 0, cv::Point());
	cv::fillConvexPoly(out_mask, contours[index], color);

	if (_debug)	filter::algos::IDPlate::showImage(out_mask);

	return workImage;
}

///*** Find and apply homography to warp image onto background image ***/
//Mat extractBackground(cv::Mat img, cv::Mat & back)
cv::Mat filter::algos::PPOC::matchImageToBackground(const cv::Mat & image, const cv::Mat & imageBackground)
{
	// Create gray copy of images
	cv::Mat imageGrayscale = filter::algos::IDPlate::convertColor2Gray(image);
	cv::Mat backgroundGrayscale = filter::algos::IDPlate::convertColor2Gray(imageBackground);
	// Create SURF detector
	int minHessian = 400;
	
	//// Compute keypoints and their descriptors for both images
	//std::vector<cv::KeyPoint> keypoints_object, keypoints_back;
	//cv::Mat descriptors_object, descriptors_back;
	//detector->detectAndCompute(image, cv::Mat(), keypoints_object, descriptors_object);
	//detector->detectAndCompute(imageBackground, cv::Mat(), keypoints_back, descriptors_back);

	cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
	std::vector<cv::KeyPoint> kptsImage, kptsBackground;
	detector->detect(imageGrayscale, kptsImage);
	detector->detect(backgroundGrayscale, kptsBackground);
	
	cv::Ptr<cv::xfeatures2d::LATCH> latch = cv::xfeatures2d::LATCH::create();
	cv::Mat descsImage, descsBackground;
	latch->compute(imageGrayscale, kptsImage, descsImage);
	latch->compute(backgroundGrayscale, kptsBackground, descsBackground);

	// Brute force matching of the descriptors
	cv::BFMatcher bfMatcher;
	std::vector<cv::DMatch> matches;
	bfMatcher.match(descsImage, descsBackground, matches);

	if (matches.empty())
	{
		throw HipeException("PPOC::matchImageToBackground - Can't find any shared keypoints in image and background");
	}

	// Get good matches for homography : only keep matches whose distance is under half of average distance
	double dist = 0;
	//for (std::vector<cv::DMatch>::iterator d_match = matches.begin(); d_match != matches.end(); ++d_match)
	//	dist += (*d_match).distance;
	for (auto & match : matches)
	{
		dist += match.distance;
	}
	double averageDist = dist / matches.size();
	double distThreshold = averageDist * 0.5;

	std::vector<cv::Point2f> p0, p1;
	//for (std::vector<cv::DMatch>::iterator d_match = matches.begin(); d_match != matches.end(); ++d_match)
	//{
	//	if ((*d_match).distance < threshold_dist)
	//	{
	//		p0.push_back(keypoints_object[(*d_match).queryIdx].pt);
	//		p1.push_back(keypoints_back[(*d_match).trainIdx].pt);
	//	}
	//}
	for (auto & match : matches)
	{
		if (match.distance < distThreshold)
		{
			p0.push_back(kptsImage.at(match.queryIdx).pt);
			p1.push_back(kptsBackground.at(match.trainIdx).pt);
		}
	}

	// Apply homography with "good" matches.
	cv::Mat transform = findHomography(p0, p1, cv::RANSAC);
	cv::Mat output;
	cv::Size s = imageGrayscale.size();
	cv::warpPerspective(image, output, transform, s);

	/*
	MatIterator_<Vec3b> it_back = back.begin<Vec3b>();
	for (MatIterator_<Vec3b> it = output.begin<Vec3b>(); it != output.end<Vec3b>(); ++it, ++it_back)
	{
	if (sum(*it) == Scalar(0.0))
	(*it_back) = Vec3b();
	}
	ShowImage(back);
	*/

	return output;
}

void filter::algos::PPOC::findObject(const cv::Mat & referenceImage, const cv::Mat & queryImage, const cv::Mat & imageMask)
{
	// Reference image will be used at findROIs output. Copy it to avoir overwritting.
	cv::Mat refImage = referenceImage.clone();
	// Use blurred grayscale image to compute contours
	cv::Mat refGray = filter::algos::IDPlate::convertColor2Gray(refImage);
	cv::Mat refGrayBlurred;
	const cv::Size blurKernel(3, 3);
	cv::blur(refGray, refGrayBlurred, blurKernel);

	// Find points of interest in reference image
	std::vector<std::vector<cv::Point>> contours = findContours(refGrayBlurred);
	std::vector<IPDesc> POIs = findROIs(refImage, imageMask, contours);

	for (auto& poi : POIs)
	{
		isObjectPresent(queryImage, poi);
	}

}

//// Old version with old names
//bool filter::algos::PPOC::isObjectPresent(const cv::Mat & queryImage, const IPDesc & poi)
//{
//	// Use grayscale blurred image to find desctiptors
//	cv::Mat queryGray = filter::algos::IDPlate::convertColor2Gray(queryImage);
//	cv::blur(queryGray, queryGray, cv::Size(3, 3));
//
//	// Image croppée de la source => roiOnRefImage
//	cv::Mat & ROIOnRef = poi.descImg();
//	if (!ROIOnRef.data)
//	{
//		throw HipeException("[ERROR] PPOC::isObjectPresent - no data in poi");
//	}
//
//	if (_debug) filter::algos::IDPlate::showImage(ROIOnRef);
//	if (_debug) filter::algos::IDPlate::showImage(queryGray);
//
//	// Detect the keypoints and extract descriptors using SURF
//	const int minHessian = 400;
//	cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);
//	std::vector<cv::KeyPoint> kptsObject, kptsScene;
//	cv::Mat descsObject, descsScene;
//	detector->detectAndCompute(ROIOnRef, cv::Mat(), kptsObject, descsObject);
//	detector->detectAndCompute(queryGray, cv::Mat(), kptsScene, descsScene);
//
//	// TM: Check descriptors validity
//	if (!descsObject.data || descsObject.empty())
//	{
//		std::cout << "no data found in object descriptor" << std::endl;
//		return false;
//	}
//	// <= Empty
//	else if (!descsScene.data || descsScene.empty())
//	{
//		std::cout << "no data found in scene descriptor" << std::endl;
//		return false;
//	}
//
//	// Debug
//	if (_debug)
//	{
//		filter::algos::IDPlate::showImage(descsObject);
//		filter::algos::IDPlate::showImage(descsScene);
//	}
//	// Matching descriptor vectors using FLANN matcher 
//	cv::FlannBasedMatcher matcher;
//	std::vector<cv::DMatch> matches;
//	matcher.match(descsObject, descsScene, matches);
//
//	// Compute min distance between keypoints
//	double minDist = std::numeric_limits<double>::max();
//	for (int i = 0; i < descsObject.rows; i++)
//	{
//		double dist = matches[i].distance;
//		if (dist < minDist)	minDist = dist;
//	}
//	if (_debug)	std::cout << "-- min dist: " << minDist;
//
//	// Draw only "good" matches according to an arbitrary criterion
//	std::vector<cv::DMatch> goodMatches;
//	for (int i = 0; i < descsObject.rows; i++)
//	{
//		if (matches[i].distance < 3 * minDist)
//			goodMatches.push_back(matches[i]);
//	}
//
//	// Localize object in scene
//	std::vector<cv::Point2f> obj;
//	std::vector<cv::Point2f> scene;
//	for (size_t i = 0; i < goodMatches.size(); i++)
//	{
//		// Get the keypoints from the good matches
//		obj.push_back(kptsObject[goodMatches[i].queryIdx].pt);
//		scene.push_back(kptsScene[goodMatches[i].trainIdx].pt);
//	}
//
//	// Draw matches
//	cv::Mat imgMatches;
//	cv::drawMatches(ROIOnRef, kptsObject, queryImage, kptsScene,
//		goodMatches, imgMatches, cv::Scalar::all(-1), cv::Scalar::all(-1),
//		std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//
//	// Test for parallel descriptor and position
//	if (!isDescriptorParallel(obj, scene))
//	{
//		std::cout << "Descriptors not parallel :: suspicious." << std::endl;
//		// Debug
//		if (_debug)
//		{
//			cv::rectangle(queryImage, poi.corner(), cv::Point(poi.corner().x + poi.width(), poi.corner().y + poi.height()), cv::Scalar(0, 0, 255));
//			filter::algos::IDPlate::showImage(imgMatches);
//		}
//	}
//	if (!isOnPosition(poi, scene))
//	{
//		std::cout << "Cannot find object in correct area." << std::endl;
//		// Debug
//		if (_debug)
//		{
//			cv::rectangle(queryImage, poi.corner(), cv::Point(poi.corner().x + poi.width(), poi.corner().y + poi.height()), cv::Scalar(255, 0, 0));
//			filter::algos::IDPlate::showImage(imgMatches);
//		}
//	}
//
//	// Debug
//	if (_debug) filter::algos::IDPlate::showImage(imgMatches);
//
//	cv::Mat homography;
//	if (!(obj.empty() || scene.empty()))	// TM: object not empty or scene empty ??
//		homography = findHomography(obj, scene, cv::RANSAC, 2);
//
//	// Get the corners from the image_1 ( the object to be "detected" )
//	std::vector<cv::Point2f> objCorners(4);
//	objCorners[0] = cvPoint(0, 0);
//	objCorners[1] = cvPoint(ROIOnRef.cols, 0);
//	objCorners[2] = cvPoint(ROIOnRef.cols, ROIOnRef.rows);
//	objCorners[3] = cvPoint(0, ROIOnRef.rows);
//
//	std::vector<cv::Point2f> sceneCorners(4);
//	if (!homography.empty())
//		cv::perspectiveTransform(objCorners, sceneCorners, homography);
//
//	// Draw lines between the corners (the mapped object in the scene - image_2 )
//	if (_debug)
//	{
//		cv::line(imgMatches, sceneCorners[0] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[1] + cv::Point2f(ROIOnRef.cols, 0),
//			cv::Scalar(0, 255, 0), 4);
//		cv::line(imgMatches, sceneCorners[1] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[2] + cv::Point2f(ROIOnRef.cols, 0),
//			cv::Scalar(0, 255, 0), 4);
//		cv::line(imgMatches, sceneCorners[2] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[3] + cv::Point2f(ROIOnRef.cols, 0),
//			cv::Scalar(0, 255, 0), 4);
//		cv::line(imgMatches, sceneCorners[3] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[0] + cv::Point2f(ROIOnRef.cols, 0),
//			cv::Scalar(0, 255, 0), 4);
//
//		// Show detected matches
//		if (_debug)
//		{
//			filter::algos::IDPlate::showImage(imgMatches); // TM: replaced line: imshow("Good Matches & Object detection", img_matches); waitKey(0);
//		}
//	}
//	return true;
//}

bool filter::algos::PPOC::isObjectPresent(const cv::Mat & queryImage, const IPDesc & poi)
{
	// Use grayscale blurred image to find desctiptors
	cv::Mat queryGray = filter::algos::IDPlate::convertColor2Gray(queryImage);
	cv::blur(queryGray, queryGray, cv::Size(3, 3));

	// Image croppée de la source => roiOnRefImage
	cv::Mat & ROIOnRef = poi.descImg();
	if (!ROIOnRef.data)
	{
		throw HipeException("[ERROR] PPOC::isObjectPresent - no data in poi");
	}

	if (_debug) filter::algos::IDPlate::showImage(ROIOnRef);
	if (_debug) filter::algos::IDPlate::showImage(queryGray);

	// Detect the keypoints and extract descriptors using SURF and LATCH
	const int minHessian = 400;
	cv::Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(minHessian);

	//	std::vector<cv::KeyPoint> kptsObject, kptsScene;
	std::vector<cv::KeyPoint> kptsQuery, kptsROI;
	detector->detect(ROIOnRef, kptsROI);
	detector->detect(queryGray, kptsQuery);

	cv::Ptr<cv::xfeatures2d::LATCH> latch = cv::xfeatures2d::LATCH::create();
	//cv::Mat descsObject, descsScene;
	cv::Mat descROI, descQuery;
	detector->detectAndCompute(ROIOnRef, cv::Mat(), kptsROI, descROI);
	detector->detectAndCompute(queryGray, cv::Mat(), kptsQuery, descQuery);

	// TM: Check descriptors validity
	if (!descROI.data || descROI.empty())
	{
		std::cout << "- no descriptors found for ROI image" << std::endl;
		return false;
	}
	// <= Empty
	else if (!descQuery.data || descQuery.empty())
	{
		std::cout << "no desctiptors found for query image" << std::endl;
		return false;
	}

	// Matching descriptor vectors using FLANN matcher 
	cv::FlannBasedMatcher matcher;
	std::vector<cv::DMatch> matches;
	matcher.match(descROI, descQuery, matches);	// <==

	// Compute min distance between keypoints
	double minDist = std::numeric_limits<double>::max();
	for (int i = 0; i < descROI.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < minDist)	minDist = dist;
	}
	if (_debug)	std::cout << "-- min dist: " << minDist;

	// Draw only "good" matches according to an arbitrary criterion
	std::vector<cv::DMatch> goodMatches;
	for (int i = 0; i < descROI.rows; i++)
	{
		if (matches[i].distance < 3 * minDist)
			goodMatches.push_back(matches[i]);
	}

	// Localize object in scene
	std::vector<cv::Point2f> goodKptsROI, goodKptsQuery;
	for (size_t i = 0; i < goodMatches.size(); i++)
	{
		// Get the keypoints from the good matches
		goodKptsROI.push_back(kptsROI[goodMatches[i].queryIdx].pt);
		goodKptsQuery.push_back(kptsQuery[goodMatches[i].trainIdx].pt);
	}

	// Draw matches
	cv::Mat imgMatches;
	cv::drawMatches(ROIOnRef, kptsROI, queryImage, kptsQuery,
		goodMatches, imgMatches, cv::Scalar::all(-1), cv::Scalar::all(-1),
		std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	// Test for parallel descriptor and position
	// TM
	if (goodKptsQuery.size() == 1)
	{
		std::cout << "- Not enough descriptors ("<< goodKptsQuery.size() << ") to qualify matching." << std::endl;
		return false;
	}

	if (!isDescriptorParallel(goodKptsROI, goodKptsQuery))
	{
		std::cout << "- Descriptors are not parallels." << std::endl;
		// Debug
		if (_debug)
		{
			cv::rectangle(queryImage, poi.corner(), cv::Point(poi.corner().x + poi.width(), poi.corner().y + poi.height()), cv::Scalar(0, 0, 255));
			filter::algos::IDPlate::showImage(imgMatches);
		}

		return false;
	}
	if (!isOnROI(goodKptsQuery, poi))
	{
		std::cout << "- query object is not within ROI" << std::endl;
		// Debug
		if (_debug)
		{
			cv::rectangle(queryImage, poi.corner(), cv::Point(poi.corner().x + poi.width(), poi.corner().y + poi.height()), cv::Scalar(255, 0, 0));
			filter::algos::IDPlate::showImage(imgMatches);
		}

		return false;
	}

	// Debug
	if (_debug) filter::algos::IDPlate::showImage(imgMatches);

	cv::Mat homography;
	if (!(goodKptsROI.empty() || goodKptsQuery.empty()))	// TM: object not empty or scene empty ??
		homography = findHomography(goodKptsROI, goodKptsQuery, cv::RANSAC, 2);

	// Get the corners from the image_1 ( the object to be "detected" )
	std::vector<cv::Point2f> objCorners(4);
	objCorners[0] = cvPoint(0, 0);
	objCorners[1] = cvPoint(ROIOnRef.cols, 0);
	objCorners[2] = cvPoint(ROIOnRef.cols, ROIOnRef.rows);
	objCorners[3] = cvPoint(0, ROIOnRef.rows);

	std::vector<cv::Point2f> sceneCorners(4);
	if (!homography.empty())
		cv::perspectiveTransform(objCorners, sceneCorners, homography);

	// Draw lines between the corners (the mapped object in the scene - image_2 )
	if (_debug)
	{
		cv::line(imgMatches, sceneCorners[0] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[1] + cv::Point2f(ROIOnRef.cols, 0),
			cv::Scalar(0, 255, 0), 4);
		cv::line(imgMatches, sceneCorners[1] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[2] + cv::Point2f(ROIOnRef.cols, 0),
			cv::Scalar(0, 255, 0), 4);
		cv::line(imgMatches, sceneCorners[2] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[3] + cv::Point2f(ROIOnRef.cols, 0),
			cv::Scalar(0, 255, 0), 4);
		cv::line(imgMatches, sceneCorners[3] + cv::Point2f(ROIOnRef.cols, 0), sceneCorners[0] + cv::Point2f(ROIOnRef.cols, 0),
			cv::Scalar(0, 255, 0), 4);

		// Show detected matches
		if (_debug)
		{
			filter::algos::IDPlate::showImage(imgMatches); // TM: replaced line: imshow("Good Matches & Object detection", img_matches); waitKey(0);
		}
	}
	return true;
}

std::vector<std::vector<cv::Point>> filter::algos::PPOC::findContours(const cv::Mat & imageGrayscale)
{
	cv::Mat canny, threshold;

	// Compute optimal threshold using Otsu's method
	const double thresh = cv::threshold(imageGrayscale, threshold, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(threshold);

	// Find edges using canny and computed threshold
	const double thresh2 = thresh;	// 1.75s
	cv::Canny(imageGrayscale, canny, thresh, thresh2, 3, true);

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(canny);

	// Find contours
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(canny, contours, hierarchy, CV_FILLED, CV_CHAIN_APPROX_SIMPLE);

	return contours;
}

std::vector<filter::algos::IPDesc> filter::algos::PPOC::findROIs(cv::Mat & out_image, const cv::Mat & imageMask, const std::vector<std::vector<cv::Point>>& contours)
{
	cv::Mat imageGrayscale = filter::algos::IDPlate::convertColor2Gray(out_image);

	int i, j;
	std::vector<IPDesc> POIs;

	// Radius and area thresholds (VS :: arbitrary values ?)
	const int minRadius = 5;
	const int maxRadius = 12;
	const int areaThreshold = 4;
	const int k = 8; // radius scaling factor

	// Iterate through each contour in contours
	for (auto& contour : contours)
	{
		// Obtain a sequence of points of contour, pointed by the variable 'contour'
		std::vector<cv::Point> points;
		cv::approxPolyDP(contour, points, cv::arcLength(cv::Mat(contour), false) * 0.015, false);

		// Bunch of commented code about basic shape vertices
		////// If there is 3 vertices in the contour (it should be a triangle)
		////if (points.size() == 3)
		////{
		////	// Drawing lines around the triangle
		////	cv::line(imageGrayscale, points[0], points[1], cv::Scalar(255, 0, 0), 4);
		////	cv::line(imageGrayscale, points[1], points[2], cv::Scalar(255, 0, 0), 4);
		////	cv::line(imageGrayscale, points[2], points[0], cv::Scalar(255, 0, 0), 4);
		////}
		//////if there is 4 vertices in the contour (it should be a quad)
		////else if (points.size() == 4)
		////{
		////	// Drawing lines around the quad
		////	cv::line(imageGrayscale, points[0], points[1], cv::Scalar(0, 255, 0), 4);
		////	cv::line(imageGrayscale, points[1], points[2], cv::Scalar(0, 255, 0), 4);
		////	cv::line(imageGrayscale, points[2], points[3], cv::Scalar(0, 255, 0), 4);
		////	cv::line(imageGrayscale, points[3], points[0], cv::Scalar(0, 255, 0), 4);
		////}
		////else if (points.size() == 6)
		////{
		//	// Drawing lines around the hexagon
		//	//cv::line(imageGrayscale, points[0], points[1], cv::Scalar(0, 255, 0), 4);
		//	cv::line(imageGrayscale, points[1], points[2], cv::Scalar(0, 255, 0), 4);
		//	cv::line(imageGrayscale, points[2], points[3], cv::Scalar(0, 255, 0), 4);
		//	cv::line(imageGrayscale, points[3], points[4], cv::Scalar(0, 255, 0), 4);
		//	cv::line(imageGrayscale, points[4], points[5], cv::Scalar(0, 255, 0), 4);
		//	cv::line(imageGrayscale, points[5], points[0], cv::Scalar(0, 255, 0), 4);
		////}
		////// If there is 7 vertices in the contour (it should be a heptagon)
		////else if (points.size() == 7)
		////{
		////	// Drawing lines around the heptagon
		////	cv::line(imageGrayscale, points[0], points[1], cv::Scalar(0, 0, 255), 4);
		////	cv::line(imageGrayscale, points[1], points[2], cv::Scalar(0, 0, 255), 4);
		////	cv::line(imageGrayscale, points[2], points[3], cv::Scalar(0, 0, 255), 4);
		////	cv::line(imageGrayscale, points[3], points[4], cv::Scalar(0, 0, 255), 4);
		////	cv::line(imageGrayscale, points[4], points[5], cv::Scalar(0, 0, 255), 4);
		////	cv::line(imageGrayscale, points[5], points[6], cv::Scalar(0, 0, 255), 4);
		////	cv::line(imageGrayscale, points[6], points[0], cv::Scalar(0, 0, 255), 4);
		////}

		if (points.size() >= 6)
		{
			// Find min enclosing circle to contour polygon and compute its area
			cv::Point2f detectedCenter;
			float detectedRadius_f;
			cv::minEnclosingCircle(cv::Mat(points), detectedCenter, detectedRadius_f);

			// Check thresholds for radius and area
			if (minRadius != 0 && detectedRadius_f < minRadius)
				continue;
			if (maxRadius != 0 && detectedRadius_f > maxRadius)
				continue;
			double realArea = cv::contourArea(points);
			if (realArea < areaThreshold)
				continue;

			int detectedRadius = static_cast<int>(detectedRadius_f);
			int kRadius = k * detectedRadius;
			cv::Size sz(kRadius, kRadius);
			cv::Mat object = cv::Mat::zeros(sz, CV_8UC1);

			cv::Point leftPoint;
			leftPoint.x = cv::max(0, static_cast<int>(detectedCenter.x - kRadius / 2));
			leftPoint.y = cv::max(0, static_cast<int>(detectedCenter.y - kRadius / 2));

			for (i = leftPoint.x; i < leftPoint.x + kRadius && i < imageGrayscale.size().width; i++)
				for (j = leftPoint.y; j < leftPoint.y + kRadius && j < imageGrayscale.size().height; j++)
				{
					int i0 = i - leftPoint.x;
					int j0 = j - leftPoint.y;
					object.at<char>(j0, i0) = imageGrayscale.at<char>(j, i);

					// Debug
					//if (_debug) filter::algos::IDPlate::showImage(object);
				}

			if (j < imageGrayscale.size().height && i < imageGrayscale.size().width)
			{
				char res = object.at<char>(1, 0);
				IPDesc desc(object.clone(), leftPoint.x, leftPoint.y, kRadius, kRadius);

				if (!isDescriptorNearImageBorder(desc, imageMask))
				{
					POIs.push_back(desc);
					cv::circle(out_image, detectedCenter, detectedRadius, cv::Scalar(0, 0, 255), 2);
				}
			}
		}
	}

	// Show the image in which identified shapes are marked
	if (_debug) filter::algos::IDPlate::showImage(out_image);

	return POIs;
}

bool filter::algos::PPOC::isDescriptorNearImageBorder(const IPDesc & descriptor, const cv::Mat & image)
{
	double nbBlack = 0;
	for (int i = 0; i < descriptor.height(); i++)
		for (int j = 0; j < descriptor.width(); j++)
		{
			if (static_cast<int>(image.at<char>(j + descriptor.corner().y, i + descriptor.corner().x)) == 0)
				nbBlack++;
		}

	const double area = descriptor.height() * descriptor.width();
	if (nbBlack / area > 0.3)
		return true;
	else
		return false;
}

bool filter::algos::PPOC::isDescriptorParallel(const std::vector<cv::Point2f> & query, const std::vector<cv::Point2f> & roi)
{
	if (roi.empty() || query.empty()) return false;
	if (roi.size() == 1 || query.size() == 1) return true;

	const double thresh = 0.6;
	const double refk0 = 0.0;
	const double refk1 = 0.0;

	// TODO :: check the object is correctly oriented into the scene before
	for (int i = 0; i < roi.size() - 1; i++)
	{
		const double x0ROI = roi[i].x;
		const double y0ROI = roi[i].y;

		const double x1ROI = roi[i + 1].x;
		const double y1ROI = roi[i + 1].y;

		const double x0Query = query[i].x;
		const double y0Query = query[i].y;

		const double x1Query = query[i + 1].x;
		const double y1Query = query[i + 1].y;

		const double dx0 = x0Query - x0ROI, dy0 = y0Query - y0ROI;
		const double dx1 = x1Query - x1ROI, dy1 = y1Query - y1ROI;

		if (abs(dx1 - dx0) > thresh || abs(dy1 - dy0) > thresh) return false;
	}

	return true;
}

bool filter::algos::PPOC::isOnROI(const std::vector<cv::Point2f>& kptsQuery, const IPDesc & poi)
{
	for (auto & keypoint : kptsQuery)
	{
		// Assert keypoint coordinates are within ROI rect
		if (keypoint.x < poi.rect().x || keypoint.x > poi.rect().x + poi.rect().width ||
			keypoint.y < poi.rect().y || keypoint.y > poi.rect().y + poi.rect().height)
			return false;
	}

	return true;
}


