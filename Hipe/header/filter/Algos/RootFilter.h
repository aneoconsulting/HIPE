#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <filter/tools/RegisterTable.h>
#include <core/HipeException.h>
#include <filter/data/ConnexData.h>


namespace filter {
	namespace Algos {
		/**
		 * \brief The RootFilter filter is the head of graph filter. Needed by every graph as a data entry point
		 */
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
			
			/**
			 * \brief Overloaded << operator. Can be used to add data to the ConnexData port. \see ConnexData
			 * \param element the data to add to the connector
			 * \return A reference to the filter
			 */
			RootFilter &operator<<(data::Data & element)
			{
				_connexData.push(element); //ICI COPY constructor data bug with fields
				return *this;
			}

			/**
			* \brief Overloaded << operator. Can be used to add images to the ConnexData port. \see ConnexData
			* \param element the image to add to the connector.
			* \return A reference to the filter
			*/
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
