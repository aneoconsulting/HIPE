#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <filter/tools/RegisterTable.h>
#include <core/HipeException.h>
#include <filter/data/ConnexData.h>


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
				//No children then pop every data from the connector
				if (_childFilters.size() == 0)
				{
					while (_connexData.size() != 0)
					{
						_connexData.pop();
					}
				}
				return HipeStatus::OK;
			}
			
			RootFilter &operator<<(data::Data & element)
			{
				_connexData.push(element); //ICI COPY constructor data bug with fields
				return *this;
			}

			RootFilter &operator<<(cv::Mat & element)
			{
				data::ImageData image(element);
				_connexData.push(image);
				return *this;
			}
		};

		ADD_CLASS(RootFilter, a);

	}
}
