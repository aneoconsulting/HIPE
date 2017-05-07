#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <filter/tools/RegisterTable.h>
#include <core/HipeException.h>
#include "data/ConnexData.h"


namespace filter {
	namespace Algos {
		class RootFilter : public filter::IFilter
		{
			//data::ConnexInput<data::Data> _connexData;
			CONNECTOR_IN(data::Data);

			REGISTER(RootFilter, ())
			{
				
			}
			REGISTER_P(int, a);

		public:
			HipeStatus process()
			{
				
				return HipeStatus::OK;
			}

			virtual RootFilter &operator<<(data::Data & element)
			{
				_connexData.push(element);
				return *this;
			}

			virtual RootFilter &operator<<(cv::Mat & element)
			{
				data::ImageData image(element);
				_connexData.push(image);
				return *this;
			}
		};

		ADD_CLASS(RootFilter, a);

	}
}
