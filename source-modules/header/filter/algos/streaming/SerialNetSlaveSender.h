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
#include "SerialNetDataSender.h"

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class SerialNetSlaveSender : public filter::IFilter
		{
			SET_NAMESPACE("vision/Cloud")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(SerialNetSlaveSender, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				skip_frame = 0;
				id = -1;
				count = 0;
				host_or_ip = "192.168.6.43";
				a_isActive = false;
				target_port = 3101;
			}

			int id;
			int count;
			

			std::atomic<bool> atomic_state;

			std::atomic<bool> a_isActive;
			SerialNetDataSender sender;

			REGISTER_P(int, _debug);

			REGISTER_P(int, skip_frame);

			REGISTER_P(std::string, host_or_ip);
			int target_port;

			std::string local_adress;

			void startSerialNetServer();

			HipeStatus process() override;



			/**
			* \brief Be sure to call the dispose method before to destroy the object SerialNetSlaveSender
			*/
			virtual void dispose();

			virtual void onStart(void* data);
		};

		ADD_CLASS(SerialNetSlaveSender, _debug, skip_frame, host_or_ip);


	}
}