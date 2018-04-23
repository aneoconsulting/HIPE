//@HIPE_LICENSE@
#pragma once
#include <coredata/NoneData.h>
#include <coredata/IODataType.h>

#include <coredata/ConnexData.h>
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
#include <corefilter/datasource/DataSource.h>


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

			VideoDataSource(const VideoDataSource &left)
			{
				this->video = left.video;
				this->a_isActive.exchange(left.a_isActive);
				this->atomic_state.exchange(left.atomic_state);
				this->url = left.url;
				this->sourceType = left.sourceType;
				this->loop = left.loop;
				this->eSourceType = left.eSourceType;
			}

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

			/**
			 * \brief Be sure to call the dispose method before to destroy the object VideoDataSource
			 */
			virtual void dispose()
			{
				if (video)
				{
					video->closeFile();
					video.reset();
					a_isActive.exchange(false);
					atomic_state.exchange(false);
				}
				
			}

			HipeStatus intialize() override;
		};

		ADD_CLASS(VideoDataSource, url, sourceType, loop);
	}
}
