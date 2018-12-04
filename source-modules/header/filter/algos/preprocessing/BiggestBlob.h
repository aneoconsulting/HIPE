//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>	

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class BiggestBlob : public filter::IFilter
		{
			SET_NAMESPACE("vision/extraction")

				CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(BiggestBlob, ()), _connexData(data::INDATA)
			{
				_debug = 0;
			}


			REGISTER_P(int, _debug);

			HipeStatus process() override;

		};

		ADD_CLASS(BiggestBlob, _debug);


	}
}