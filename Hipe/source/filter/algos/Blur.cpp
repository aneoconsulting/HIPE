#include "filter/algos/Blur.h"
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


		HipeStatus Blur::process()
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


				blur(inputImage, outputImage, cv::Size(kernelSize, kernelSize)); //gaussian filter


				_connexData.push(outputImage); //push output image
			}
			return OK;
		}
	}
}