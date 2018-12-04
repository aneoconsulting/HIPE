//@HIPE_LICENSE@
#include <filter/algos/preprocessing/Median.h>
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


				medianBlur(inputImage, outputImage, kernelSize); //gaussian filter


				PUSH_DATA(data::ImageData(outputImage)); //push output image
			}
			return OK;
		}
	}
}