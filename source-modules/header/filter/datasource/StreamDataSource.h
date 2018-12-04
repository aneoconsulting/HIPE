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
		class FILTER_EXPORT StreamDataSource : public IFilter, public DataSource
		{
			CONNECTOR(data::NoneData, data::Data);

			SET_NAMESPACE("vision/datasource")

				REGISTER(StreamDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::VIDF;
				atomic_state = false;
				a_isActive = false;
			}

			REGISTER_P(int, udp_port);

			REGISTER_P(std::string, sourceType);

			data::IODataType eSourceType;

			StreamDataSource(const StreamDataSource &left)
			{
				this->video = left.video;
				this->a_isActive.exchange(left.a_isActive);
				this->atomic_state.exchange(left.atomic_state);
				this->udp_port = left.udp_port;
				this->sourceType = left.sourceType;
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
			* \brief Be sure to call the dispose method before to destroy the object StreamDataSource
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

		ADD_CLASS(StreamDataSource, udp_port, sourceType);
	}
}
