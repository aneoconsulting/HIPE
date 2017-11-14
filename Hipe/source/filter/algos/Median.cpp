#include "filter/algos/Median.h"
#include "data/ShapeData.h"
#include "data/ImageData.h"


namespace filter
{
	namespace algos
	{
		/**
		* \var Blur::kernelsize
		* The size of the kernel to use (i.e. the number of neighbouring pixels to evaluate).
		*/


		HipeStatus Median::process()
		{


			while (!_connexData.empty())
			{

				data::ImageData data = _connexData.pop();//Pop input image
				cv::Mat const inputImage = data.getMat();
				cv::Mat outputImage;

				if (!inputImage.data)
				{
					throw HipeException("[Error] BilateralFilter::process - No input data found.");
				}


				medianBlur(inputImage, outputImage, kernelsize); //gaussian filter


				_connexData.push(outputImage); //push output image
			}
			return OK;
		}
	}
}