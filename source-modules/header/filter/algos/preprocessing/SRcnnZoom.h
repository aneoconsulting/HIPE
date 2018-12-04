#pragma once

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
		class SRcnnZoom : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			SET_NAMESPACE("vision/preprocessing")

			REGISTER(SRcnnZoom, ()), _connexData(data::INDATA)
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
			HipeStatus process();

		};

		ADD_CLASS(SRcnnZoom, ratio, width, height);
	}
}
