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
#include <data/FileImageData.h>
#include <data/VideoData.h>
#include <filter/datasource/DataSource.h>

namespace filter
{
	
	namespace datasource
	{
		class FILTER_EXPORT VideoDataSource : public IFilter, public DataSource
		{
			CONNECTOR(data::NoneData, data::Data);

			SET_NAMESPACE("vision/datasource")

			REGISTER(VideoDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::VIDF;
				loop = false;
				atomic_state = false;
				a_isActive = false;
			}

			REGISTER_P(std::string, url);
		
			REGISTER_P(std::string, sourceType);

			REGISTER_P(bool, loop);

			data::IODataType eSourceType;

			virtual data::IODataType getSourceType() const
			{
				return eSourceType;
			}

			void setSourceType(data::IODataType source_type)
			{
				eSourceType = source_type;
			}

			std::shared_ptr<data::IVideo> video;

			std::atomic<bool> atomic_state;

			std::atomic<bool> a_isActive;

			inline std::atomic<bool> & isActive() 
			{
				return a_isActive;
			}

			void setactive()
			{
				a_isActive.exchange(true);
			}

			HipeStatus process();
		};

		ADD_CLASS(VideoDataSource, url, sourceType, loop);
	}
}
