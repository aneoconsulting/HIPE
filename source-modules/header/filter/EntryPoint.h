//@HIPE_LICENSE@
#pragma once


#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/IFilter.h>
#include <corefilter/tools/RegisterTable.h>
#include <core/HipeException.h>
#include <coredata/ConnexData.h>
#include <coredata/NoneData.h>
#include <corefilter/filter_export.h>


namespace filter {
	namespace algos {
		/**
		* \brief The RootFilter filter is the head of graph filter. Needed by every graph as a data entry point.
		*/
		class FILER_EXPORT EntryPoint : public filter::IFilter
		{
			//data::ConnexInput<data::Data> _connexData;
			CONNECTOR_IN(data::NoneData);

			REGISTER(EntryPoint, ())
			{

			}
			REGISTER_P(int, unused);

		public:
			HipeStatus process()
			{
				////No children then pop every data from the connector
				//if (_childFilters.size() == 0)
				//{
				//	while (_connexData.size() != 0)
				//	{
				//		_connexData.pop();
				//	}
				//}
				return HipeStatus::OK;
			}

			/**
			* \brief Overloaded << operator. Can be used to add data to the ConnexData port. \see ConnexData
			* \param element the data to add to the connector.
			* \return A reference to the filter.
			*/
			EntryPoint &operator<<(data::Data & element)
			{
				PUSH_DATA(data::NoneData()); //Here COPY constructor data bug with fields
				return *this;
			}

			/**
			* \brief Overloaded << operator. Can be used to add images to the ConnexData port. \see ConnexData
			* \param element the image to add to the connector.
			* \return A reference to the filter.
			*/
			EntryPoint &operator<<(cv::Mat & element)
			{
				
				return *this;
			}
		};

		ADD_CLASS(RootFilter, unused);

	}
}
