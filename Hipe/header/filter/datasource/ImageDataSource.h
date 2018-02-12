#pragma once
#include <data/NoneData.h>
#include <data/IODataType.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
#include <data/ConnexData.h>
#include <corefilter/tools/filterMacros.h>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <core/HipeStatus.h>
#include <corefilter/Model.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>

#include <string>
#include "data/FileImageData.h"
#include "data/VideoData.h"

namespace filter
{
	
	namespace datasource
	{
		class FILTER_EXPORT ImageDataSource : public filter::IFilter
		{
			CONNECTOR(data::NoneData, data::Data);

			SET_NAMESPACE("vision/datasource")

			REGISTER(ImageDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::IMGF;
				loop = false;
				atomic_state = false;
			}

			REGISTER_P(std::string, url);
		
			REGISTER_P(std::string, sourceType);

			REGISTER_P(bool, loop);

			data::IODataType eSourceType;

			data::ImageArrayData imgs;

			std::atomic<bool> atomic_state;

			HipeStatus process();
		};

		ADD_CLASS(ImageDataSource, url, sourceType, loop);
	}
}
