//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#pragma warning(pop)



namespace filter
{
	namespace algos
	{
		/**
		 * \brief The AverageColor filter will compute the average color of an image in RGB space.
		 *		  Note that the average color computed will be outputed as an image of 1 pixel.
		 */
		class AverageColor : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(AverageColor, ()), _connexData(data::INDATA)
			{

			}
			REGISTER_P(int, unused);

		public:
			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat img = data.getMat();

				cv::Scalar averageColor = cv::mean(img);
				cv::Mat output(cv::Size(1, 1), img.type(), averageColor);

				PUSH_DATA(data::ImageData(output));
				return OK;
			}
		};

		ADD_CLASS(AverageColor, unused);
	}
}
