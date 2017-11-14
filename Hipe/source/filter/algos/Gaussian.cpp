#include "filter/algos/Gaussian.h"
#include "data/ShapeData.h"
#include "data/ImageData.h"


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

				data::ImageData images(_connexData.pop()); //Pop input image

				cv::Mat res;

				GaussianBlur(images.getMat(), res, cv::Size(kernelsize, kernelsize), sigmaX, sigmaY); //gaussian filter


				_connexData.push(res); //push output image
			}
			return OK;
		}
	}
}