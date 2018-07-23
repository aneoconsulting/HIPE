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

#include <algos/detection/LargestContour.h>
#include "data/FileVideoInput.h"
#include "algos/IDPlate/IDPlateCropper.h"


static
bool
compare_contour_area(std::vector<cv::Point> const & a, std::vector<cv::Point> const & b)
{
	return cv::contourArea(a) > cv::contourArea(b);
}

std::vector<cv::Point> contoursConvexHull(std::vector<std::vector<cv::Point> > contours)
{
	std::vector<cv::Point> result;
	std::vector<cv::Point> pts;
	for (size_t i = 0; i< contours.size(); i++)
		for (size_t j = 0; j< contours[i].size(); j++)
			pts.push_back(contours[i][j]);
	cv::convexHull(pts, result);
	return result;
}

HipeStatus filter::algos::LargestContour::process()
{
	data::ImageData data = _connexData.pop();

	if (data.getMat().channels() != 1)
		throw HipeException("LargestContour cannot process other than a N/B image");

	unsigned char threshold = 128;
	float LargestContourArea;
	cv::Scalar maskColor(64, 0, 0);

	// Dilate image to enlarge found contours
	cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
	dilate(data.getMat(), data.getMat(), dilateKernel);


	if (_debug >= 1) IDPlate::showImage(data.getMat(), "debug", false, -1);

	//cv::Mat mask;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(data.getMat(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	cv::findContours(data.getMat(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	cv::Mat result;
	cv::Mat temp = cv::Mat::zeros(data.getMat().size(), data.getMat().type());
	cv::Mat temp2 = cv::Mat::zeros(data.getMat().size(), data.getMat().type());
	// CV_FILLED fills the connected components found
	if (contours.size() != 0)
	{
		std::sort(contours.begin(), contours.end(), compare_contour_area);
		



		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(temp, contours, i, cv::Scalar(255), CV_FILLED);
			/*double epsilon = 0.1*cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], temp, epsilon, true);*/
			
		}

		std::vector<cv::Point> ConvexHullPoints = contoursConvexHull(contours);
		//drawContours(temp, ConvexHullPoints, -1, cv::Scalar(255), CV_FILLED);
		polylines(temp, ConvexHullPoints, true, cv::Scalar(0, 0, 255), 2);
	}
	dilate(temp, temp, dilateKernel);
	cv::findContours(temp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	if (contours.size() != 0)
	{
		
		std::sort(contours.begin(), contours.end(), compare_contour_area);
		drawContours(temp2, contours, 0, cv::Scalar(255), 1);
		std::vector<cv::Point> ConvexHullPoints = contoursConvexHull(contours);
		polylines(temp2, ConvexHullPoints, true, cv::Scalar(0, 0, 255), 2);
		cv::findContours(temp2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		std::sort(contours.begin(), contours.end(), compare_contour_area);
		drawContours(temp2, contours, 0, cv::Scalar(255), 1);
	}


	PUSH_DATA(data::ImageData(temp2));

	return OK;
}
