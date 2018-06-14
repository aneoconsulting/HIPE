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

#include <algos/preprocessing/BiggestBlob.h>
#include "data/FileVideoInput.h"


cv::Point findBiggestBlobPos(cv::Mat& binaryImage, cv::Scalar fillColor, cv::Scalar biggestBlobFillColor, unsigned char threshold, float& out_blobArea, int debug)
{
	cv::Mat outputImage = binaryImage;

	int maxArea = -1;
	cv::Point maxAreaPos(-1, -1);

	// For each pixel, if pixel color is greater than threshold, it's a blob : paint it black. The biggest found area is the whole plate
	for (int y = 0; y < outputImage.size().height; y++)
	{
		const uchar* row = outputImage.ptr<uchar>(y);

		for (int x = 0; x < outputImage.size().width; x++)
		{
			if (row[x] >= threshold)
			{
				cv::Point pos(x, y);
				int currArea = cv::floodFill(outputImage, pos, fillColor);

				if (currArea > maxArea)
				{
					// Fill max area in max area color
					cv::floodFill(outputImage, pos, biggestBlobFillColor);

					// Fill old max area in fill color
					if (maxAreaPos != cv::Point(-1, -1)) cv::floodFill(outputImage, maxAreaPos, fillColor);

					// Update max area value
					maxArea = currArea;

					// Update max area pos
					maxAreaPos = pos;

					// Debug
				}
			}
		}
	}



	out_blobArea = maxArea;
	return maxAreaPos;
}

void showImage(const cv::Mat & image, std::string name, bool shouldDestroy, int waitTime)
{
	cv::namedWindow(name);
	cv::imshow(name, image);
	if (waitTime >= 0)	cv::waitKey(waitTime);
	if (shouldDestroy)	cv::destroyWindow(name);
}

static
bool
compare_contour_area(std::vector<cv::Point> const & a, std::vector<cv::Point> const & b)
{
	return cv::contourArea(a) > cv::contourArea(b);
}

HipeStatus filter::algos::BiggestBlob::process()
{
	data::ImageData data = _connexData.pop();

	if (data.getMat().channels() != 1)
		throw HipeException("BiggestBlob cannot process other than a N/B image");

	unsigned char threshold = 128;
	float biggestBlobArea;
	cv::Scalar maskColor(64, 0, 0);

	// Dilate image to enlarge found contours
	cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
	dilate(data.getMat(), data.getMat(), dilateKernel);
	

	if (_debug >= 1) showImage(data.getMat(), "debug", false, -1);

	//cv::Mat mask;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(data.getMat(), contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0, 0));
	cv::findContours(data.getMat(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	if (_debug >= 2) showImage(data.getMat(), "debug_contours", false, -1);

	// CV_FILLED fills the connected components found
	if (contours.size() != 0)
	{
		std::sort(contours.begin(), contours.end(), compare_contour_area);
		drawContours(data.getMat(), contours, -1, cv::Scalar(255), CV_FILLED);

		

		for (int i = 0; i < contours.size(); i++)
		{
			double epsilon = 0.1*cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], data.getMat(), epsilon, true);
		}
	}
	cv::Point biggestBlobPos = findBiggestBlobPos(data.getMat(), maskColor, maskColor, threshold, biggestBlobArea, false);

	// Paint back biggest blob in white
	if (biggestBlobPos.x >= 0 && biggestBlobPos.y >= 0)
		cv::floodFill(data.getMat(), biggestBlobPos, cv::Scalar(255, 255, 255));

	// Paint everything but biggest blob in black
	for (int y = 0; y < data.getMat().size().height; y++)
	{
		uchar* row = data.getMat().ptr(y);
		for (int x = 0; x < data.getMat().size().width; x++)
		{
			cv::Point currentPixel(x, y);
			if (row[x] == maskColor[0] && currentPixel != biggestBlobPos)
			{
				int area = floodFill(data.getMat(), currentPixel, cv::Scalar(0, 0, 0));
			}
		}
	}

	PUSH_DATA(data::ImageData(data.getMat()));

	return OK;
}
