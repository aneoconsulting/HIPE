//@HIPE_LICENSE@

#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>

#include <data/ImageData.h>
#include "SerialNetDataSender.h"
#include <mutex>

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class PopGraphToNode : public filter::IFilter
		{
			typedef boost::asio::ip::tcp::socket TCP;

			SET_NAMESPACE("vision/Cloud")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(PopGraphToNode, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				skip_frame = 0;
				address = "192.168.6.43";
				a_isActive = false;
			}


			std::atomic<bool> a_isActive;
			std::shared_ptr<HipeConnection> connector;
			std::shared_ptr<boost::asio::io_service> service;
		public:
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;
			std::mutex socket_mutex;
			std::unique_ptr<boost::thread> thr_client;

			REGISTER_P(int, _debug);

			REGISTER_P(int, skip_frame);

			REGISTER_P(std::string, address);
			REGISTER_P(int, port);

			inline std::atomic<bool> & isActive()
			{
				return a_isActive;
			}

			void setactive()
			{
				a_isActive.exchange(true);
			}

			std::shared_ptr<boost::asio::deadline_timer> get_timeout_timer(int nb_seconds);

			void Connect();

			HipeStatus process() override;
			void stop();


			/**
			* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
			*/
			virtual void dispose();
		};

		ADD_CLASS(PopGraphToNode, _debug, skip_frame, address);


	}
}