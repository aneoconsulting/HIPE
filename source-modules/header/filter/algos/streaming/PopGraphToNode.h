//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>	
#include "data/VideoData.h"

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class PopGraphToNode : public filter::IFilter
		{
			SET_NAMESPACE("vision/Cloud")

				CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(PopGraphToNode, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				skip_frame = 0;
				id = -1;
				count = 0;
				host_or_ip = "192.168.6.43";
				sync = 0;
				store_video = "store_cloud_result.avi";
			}

			int id;
			int count;
			std::string store_video;
			std::shared_ptr<data::IVideo> video;
			std::atomic<bool> a_isActive;
			std::atomic<bool> atomic_state;

			REGISTER_P(int, _debug);

			REGISTER_P(int, skip_frame);

			REGISTER_P(int, sync);

			REGISTER_P(std::string, host_or_ip);

			inline std::atomic<bool> & isActive()
			{
				return a_isActive;
			}

			void setactive()
			{
				a_isActive.exchange(true);
			}

			int getLastKnownIDFromDirectory();

			HipeStatus receiveStream();

			HipeStatus process() override;


			/**
			* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
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
		};

		ADD_CLASS(PopGraphToNode, _debug, skip_frame, sync, host_or_ip);


	}
}