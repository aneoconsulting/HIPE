//@HIPE_LICENSE@
#pragma once
#ifdef USE_WEBRTC
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
#include <WebRTCCapturer.h>


namespace filter
{

	namespace datasource
	{
		class FILTER_EXPORT WebRTCVideoDataSource : public IFilter, public DataSource
		{
			CONNECTOR(data::NoneData, data::Data);

			SET_NAMESPACE("vision/datasource")

				REGISTER(WebRTCVideoDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::VIDF;
				loop = false;
				atomic_state = false;
				a_isActive = false;
				video = nullptr;
				task = nullptr;
			}

			REGISTER_P(int, port);

			std::string sourceType;

			REGISTER_P(bool, loop);

			data::IODataType eSourceType;

			WebRTCVideoDataSource(const WebRTCVideoDataSource& left);
			void captureTasks();

			virtual data::IODataType getSourceType() const
			{
				return eSourceType;
			}

			void setSourceType(data::IODataType source_type)
			{
				eSourceType = source_type;
			}

			WebRTCCapturer * video;
			std::thread * task;
			core::queue::ConcurrentQueue<cv::Mat> stack;

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
			* \brief Be sure to call the dispose method before to destroy the object WebRTCVideoDataSource
			*/
			void dispose() override;

			HipeStatus intialize() override;

			void onLoad(void *data) override;
			

		};

		ADD_CLASS(WebRTCVideoDataSource, port, loop);
	}
}
#endif // USE_WEBRTC