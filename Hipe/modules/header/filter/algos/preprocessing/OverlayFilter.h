//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/ShapeData.h>
#include <data/PatternData.h>
#include <data/DirPatternData.h>


namespace filter
{
	namespace algos
	{
		/**
		* \todo
		* \brief The OverlayFilter filter is used contour regions of interests in an image.
		*
		* The ConnexData port must contain 2 objects. The image on which the filter will draw, and the list of regions of interest (ShapeData object), or text.
		*/
		class OverlayFilter : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::Data, data::ImageData);

			REGISTER(OverlayFilter, ()), _connexData(data::INDATA)
			{
				asReference = true;
			}

			REGISTER_P(bool, asReference);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process();

			bool isDrawableSource(const data::Data& data);
			bool isOverlayData(const data::Data& data);

			data::ImageData extractSourceImageData(data::Data& data);

			void drawShape(cv::Mat& image, const data::ShapeData& shape);

		};

		ADD_CLASS(OverlayFilter, asReference);
	}
}
