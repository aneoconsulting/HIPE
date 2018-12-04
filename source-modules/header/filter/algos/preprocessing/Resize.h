//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc/imgproc.hpp>
#pragma warning(pop)

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

			REGISTER(Resize, ()), _connexData(data::INDATA)
			{
				ratio = 1;
				width = 0;
				height = 0;
				inoutData = true;
			}

			REGISTER_P(double, ratio);
			REGISTER_P(int, width);
			REGISTER_P(int, height);
			REGISTER_P(bool, inoutData);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process();
		};

		ADD_CLASS(Resize, ratio, width, height, inoutData) ;
	}
}
