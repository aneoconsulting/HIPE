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

#include <filter/algos/show/ConcatToShow.h>
#include <opencv2/videostab/wobble_suppression.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat filter::algos::ConcatToShow::ShowManyImages(std::vector<data::ImageData> arrayData) const
{
	cv::Size size;
	int i;
	
	int x, y;
	int nArgs= arrayData.size();

	// w - Maximum number of images in a row
	// h - Maximum number of images in a column
	int w, h;

	// scale - How much we have to resize the image
	float scale;
	int max;

	// If the number of arguments is lesser than 0 or greater than 12
	// return without displaying
	if (nArgs <= 0)
	{
		throw HipeException("Number of arguments too small....\n");
	}
	else if (nArgs > 14)
	{
		throw HipeException("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
	}
		// Determine the size of the image,
		// and the number of rows/cols
		// from number of arguments
	else if (nArgs == 1)
	{
		w = h = 1;
		size = arrayData[0].getMat().size();
	}
	else if (nArgs == 2)
	{
		w = 2;
		h = 1;
	}
	else if (nArgs == 3 || nArgs == 4)
	{
		w = 2;
		h = 2;
		
	}
	else if (nArgs == 5 || nArgs == 6)
	{
		w = 3;
		h = 2;
		
	}
	else if (nArgs == 7 || nArgs == 8)
	{
		w = 4;
		h = 2;
		
	}
	else
	{
		w = 4;
		h = 3;
	}

	// Create a new 3 channel image
	cv::Mat disp_image = cv::Mat::zeros(height, width, CV_8UC3);

	int width_avg = width / w;
	int height_avg = height / h;

	int hpix_pos = 0;
	int imgIdx = 0;
	int n = 0;
	int nbImg = arrayData.size();

	for (int h_cur = 0; h_cur < h; h_cur++)
	{
		int wpix_pos = 0;
		int m = 0;
		for (int w_cur = 0; w_cur < w && imgIdx < nbImg; w_cur++, imgIdx++)
		{
			cv::Mat image = arrayData[imgIdx].getMat();
			if (image.empty()) continue;

			int x = image.size().width;
			int y = image.size().height;
			double scale_w = std::max(((double)x) / (double)(width_avg), 0.1);
			double scale_h = std::max((double)y / (double)(height_avg), 0.1);
			
			//Respect form of source image
			double scale = std::max(scale_w, scale_h);

			cv::Rect ROI(m, n, (int)(x / scale), (int)(y / scale));
			cv::Mat temp;
			cv::resize(image, temp, cv::Size(ROI.width, ROI.height));
			temp.copyTo(disp_image(ROI));
			m += width_avg;
		}
		n += height_avg;
	}

	return disp_image;
}

bool sortLabels(data::ImageData data1, data::ImageData data2)
{
	auto d1 = data1.getLabel();
	auto d2 = data2.getLabel();
	auto res = d1 < d2;
	return res;
}

HipeStatus filter::algos::ConcatToShow::process()
{
	std::vector<cv::Mat> arrayData;
	std::vector<data::ImageData> arrayImages;
	while (!_connexData.empty())
	{
		data::Data img = static_cast<data::Data>(_connexData.pop());
		if (data::DataTypeMapper::isImage(img.getType()))
		{
			data::ImageData image_data = static_cast<data::ImageData&>(img);
			auto label = image_data.getLabel();
			arrayImages.push_back(image_data);
			/*arrayData.push_back(image_data.getMat());
			img.getLabel()*/
		}
	}
	std::sort(arrayImages.begin(), arrayImages.end(), sortLabels);

	cv::Mat show_many_images = ShowManyImages(arrayImages);

	PUSH_DATA(data::ImageData(show_many_images));


	return OK;
}
