#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>

#include <opencv2/imgproc/imgproc.hpp>

namespace filter
{
	namespace algos
	{
		 /**
		 * \var Resize::ratio
		 * The factor by which the image must be rescaled.
		 *
		 * \var Resize::width
		 * If a width and height is provided the image will be rescaled to these values. The width to which the image must be rescaled.
		 *
		 * \var Resize::height
		 * If a width and height is provided the image will be rescaled to these values. The height to which the image must be rescaled.
		 *
		 * \brief The Resize filter will resize an image (downscale or upscale).
		 */
		class Resize : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			SET_NAMESPACE("vision/preprocessing")

			REGISTER(Resize, ()), _connexData(data::INOUT)
			{
				ratio = 1;
				width = 0;
				height = 0;
			}

			REGISTER_P(double, ratio);
			REGISTER_P(int, width);
			REGISTER_P(int, height);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				while (!_connexData.empty()) // While i've parent data
				{
					auto images = _connexData.pop();
					if (images.getType() == data::PATTERN)
					{
						throw HipeException("The resize object cant resize PatternData. Please Develop ResizePatterData");
					}

					//Resize all images coming from the same parent
					for (auto &myImage : images.Array())
					{
						if (width == 0 || height == 0)
						{
							int l_iwidth = myImage.cols;
							int l_iheight = myImage.rows;
							cv::Size size(l_iwidth / ratio, l_iheight / ratio);

							cv::resize(myImage, myImage, size, 0.0, 0.0, cv::INTER_CUBIC);
						}
						else
						{
							cv::Size size(width, height);
							cv::resize(myImage, myImage, size, 0.0, 0.0, cv::INTER_CUBIC);
						}
					}
				}
				return OK;
			}
		};

		ADD_CLASS(Resize, ratio, width, height);
	}
}
