#include <filter/algos/show/ConcatToShow.h>
#include <opencv2/videostab/wobble_suppression.hpp>

cv::Mat filter::algos::ConcatToShow::ShowManyImages(std::vector<cv::Mat> arrayMat)
{
	cv::Size size;
	int i;
	
	int x, y;
	int nArgs= arrayMat.size();

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
		size = arrayMat[0].size();
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
	int nbImg = arrayMat.size();

	for (int h_cur = 0; h_cur < h; h_cur++)
	{
		int wpix_pos = 0;
		int m = 0;
		for (int w_cur = 0; w_cur < w && imgIdx < nbImg; w_cur++, imgIdx++)
		{
			int x = arrayMat[imgIdx].size().width;
			int y = arrayMat[imgIdx].size().height;
			int scale_w = std::max((int)std::ceil(((double)x) / (double)(width_avg)), 1);
			int scale_h = std::max((int)std::ceil(((double)y) / (double)(height_avg)), 1);
			
			//Respect form of source image
			int scale = std::max(scale_w, scale_h);

			cv::Rect ROI(m, n, (int)(x / scale), (int)(y / scale));
			cv::Mat temp;
			cv::resize(arrayMat[imgIdx], temp, cv::Size(ROI.width, ROI.height));
			temp.copyTo(disp_image(ROI));
			m += width_avg;
		}
		n += height_avg;
	}

	return disp_image;
}

HipeStatus filter::algos::ConcatToShow::process()
{
	std::vector<cv::Mat> arrayData;

	while (!_connexData.empty())
	{
		data::Data img = _connexData.pop();
		if (data::DataTypeMapper::isImage(img.getType()))
		{
			data::ImageData image_data = static_cast<data::ImageData>(img);
			arrayData.push_back(image_data.getMat());
		}
	}
	cv::Mat show_many_images = ShowManyImages(arrayData);
	
	data::ImageData result = data::ImageData(show_many_images);

	PUSH_DATA(result);


	return OK;
}
