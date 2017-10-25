#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/PatternData.h>

namespace filter
{
	namespace algo {

		class OrderCircles : public filter::IFilter
		{
			CONNECTOR(data::ListIOData, data::ImageArrayData);

			REGISTER(OrderCircles, ()), _connexData(data::INOUT)
			{
			}


			virtual std::string resultAsString() { return std::string("TODO"); };
		public:
			HipeStatus process();
			void order_circles_by_grid(std::vector<cv::Point2f> const & points);
		};
		ADD_CLASS(OrderCircles);
	}
}
