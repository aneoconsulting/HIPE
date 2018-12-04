//@HIPE_LICENSE@
#include <filter/algos/preprocessing/Gaussian.h>
#include <data/ShapeData.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		/**
		* \var Blur::kernelsize
		* The size of the kernel to use (i.e. the number of neighbouring pixels to evaluate).
		*
		* \var Blur::sigmaX
		* The gaussian kernel standard deviation in the X direction.
		*
		* \var Blur::sigmaY
		* The gaussian kernel standard deviation in the Y direction. Default value is sigmaY=sigmaX
		*/


		HipeStatus Gaussian::process()
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


				GaussianBlur(inputImage, outputImage, cv::Size(kernelSize, kernelSize), sigmaX, sigmaY); //gaussian filter


				PUSH_DATA(data::ImageData(outputImage)); //push output image
			}
			return OK;
		}
	}
}