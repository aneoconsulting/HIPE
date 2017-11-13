#include "filter/algos/Gaussian.h"
#include "data/ShapeData.h"
#include "data/ImageData.h"


namespace filter
{
	namespace algos
	{

		HipeStatus Gaussian::process()
		{


			while (!_connexData.empty()) // While i've parent data
			{

				data::ImageData images(_connexData.pop());
				cv::Mat res;

				/// Reduce noise with a kernel 3x3
				GaussianBlur(images.getMat(), res, cv::Size(-1, -1), sigma, sigma);


				_connexData.push(res);
			}
			return OK;
		}
	}
}