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
		class PushGraphToNode : public filter::IFilter
		{
			SET_NAMESPACE("vision/Cloud")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(PushGraphToNode, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				skip_frame = 0;
				id = -1;
				count = 0;
				host_or_ip = "192.168.6.43";
			}

			int id;
			int count;
			std::string store_video;
			std::shared_ptr<data::IVideo> video;

			std::atomic<bool> atomic_state;

			std::atomic<bool> a_isActive;

			REGISTER_P(int, _debug);

		

			REGISTER_P(int, skip_frame);

			REGISTER_P(std::string, host_or_ip);

			

			int getLastKnownIDFromDirectory();

			HipeStatus SendVideo(const cv::Mat& picture);

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

		ADD_CLASS(PushGraphToNode, _debug, skip_frame, host_or_ip);


	}
}