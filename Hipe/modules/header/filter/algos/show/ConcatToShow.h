#pragma once

#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class ConcatToShow : public IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageData);

			SET_NAMESPACE("vision/show");

			REGISTER(ConcatToShow, ()), _connexData(data::INDATA)
			{
				width = 1280;
				height = 720;
				nb_img_by_row = 1;
			}

			REGISTER_P(int, width);
			REGISTER_P(int, height);

			REGISTER_P(int, nb_img_by_row);

			
			~ConcatToShow()
			{
			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		public:
			HipeStatus process();

			cv::Mat ShowManyImages(std::vector<data::ImageData> arrayMat) const;
		};

		ADD_CLASS(ConcatToShow, width, height, nb_img_by_row);
	}
}
