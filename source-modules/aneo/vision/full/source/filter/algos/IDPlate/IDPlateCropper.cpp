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

#include <filter/algos/IDPlate/IDPlateCropper.h>
#include <core/HipeStatus.h>

HipeStatus filter::algos::IDPlateCropper::process()
{
	while (!_connexData.empty())
	{
		data::ImageArrayData data = _connexData.pop();
		
		data::ImageArrayData output;
		for (auto &image : data.Array_const())
		{
			cv::Mat roi = processPlateImage(image);
			output << roi;
		}
		PUSH_DATA(output);
	}
	return OK;
}

cv::Mat filter::algos::IDPlateCropper::processPlateImage(const cv::Mat & plateImage)
{
	// Bilateral filtering to smooth images and reduce noise
	const int bDiameter = 31;
	cv::Mat output = filter::algos::IDPlate::applyBilateralFiltering(plateImage, bfilterPasses, bDiameter, bDiameter * 2, bDiameter * 0.5, _debug, false);

	// Convert image to grayscale
	output = filter::algos::IDPlate::convertColor2Gray(output);

	// Convert image to binary (black/white)
	cv::adaptiveThreshold(output, output, 255, CV_ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 31, -2);

	// Find edges using canny algorithm
	cv::Canny(output.clone(), output, 450, 100);

	// Dilate image to enlarge found contours
	cv::Mat dilateKernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5, 5));
	dilate(output, output, dilateKernel);

	// Color blobs on image and find biggest blob
	cv::Scalar maskColor(64, 0, 0);
	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

	//cv::Point biggestBlobPos = maskBlobs(output, maskColor);
	unsigned char threshold = 128;
	float biggestBlobArea;
	cv::Point biggestBlobPos = filter::algos::IDPlate::findBiggestBlobPos(output, maskColor, maskColor, threshold, biggestBlobArea, _debug);

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

	// Paint back biggest blob in white
	cv::floodFill(output, biggestBlobPos, cv::Scalar(255, 255, 255));

	// Paint everything but biggest blob in black
	for (int y = 0; y < output.size().height; y++)
	{
		uchar* row = output.ptr(y);
		for (int x = 0; x < output.size().width; x++)
		{
			cv::Point currentPixel(x, y);
			if (row[x] == maskColor[0] && currentPixel != biggestBlobPos)
			{
				int area = floodFill(output, currentPixel, cv::Scalar(0, 0, 0));
			}
		}
	}

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

	// Find contours of biggest blob:
	cv::Rect RegionToCrop = cv::boundingRect(output);

	// Add margin
	const double ratio = 0.05;
	int marginWidth = ratio * plateImage.cols;
	int marginHeight = ratio * plateImage.rows;

	RegionToCrop.width += marginWidth;
	RegionToCrop.height += marginHeight;

	// Rescale if coordinates out of image
	if (RegionToCrop.x + RegionToCrop.width > output.cols)	RegionToCrop.width = output.cols - RegionToCrop.x;
	if (RegionToCrop.y + RegionToCrop.height > output.rows) RegionToCrop.height = output.rows - RegionToCrop.y;


	output = plateImage(RegionToCrop);

	// Debug
	if (_debug)	filter::algos::IDPlate::showImage(output);

	return output;
}
