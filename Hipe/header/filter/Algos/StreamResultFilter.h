#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/tools/RegisterTable.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <core/queue/ConcurrentQueue.h>
#include <opencv2/highgui/highgui.hpp>
#include <streaming/Streaming.h>
#include <filter/tools/filterMacros.h>
#include <filter/data/ImageData.h>


namespace filter
{
	namespace algos
	{
		class StreamResultFilter : public IFilter
		{
			//data::ConnexOutput<filter::data::ImageData> connData;
			CONNECTOR_OUT(data::ImageData);

			core::queue::ConcurrentQueue<filter::data::ImageData> queue;
			struct timeval current_time;
			int fps_avg;
			int nb_frame;
			

			// second part of sender pipeline
			std::stringstream uri;
			cv::VideoWriter writer;

			/* int setenv(const char *name, const char *value, int overwrite) */
			/* { */
			/* 	int errcode = 0; */
			/* 	if (!overwrite) { */
			/* 		size_t envsize = 0; */
			/* 		errcode = getenv_s(&envsize, NULL, 0, name); */
			/* 		if (errcode || envsize) return errcode; */
			/* 	} */
			/* 	return _putenv_s(name, value); */
			/* } */

			REGISTER(StreamResultFilter, ())
			{
				port = 8864;
				current_time.tv_sec = 0;
				current_time.tv_usec = 0;
				fps_avg = 0;
				nb_frame = 0;
				
				//setenv("GST_DEBUG", "cat:level...", 1);
				//uri << "appsrc !videoconvert ! x264enc noise - reduction = 10000 tune = zerolatency byte - stream = true threads = 4 ! mpegtsmux ! udpsink host = localhost port = ";
				//uri << "appsrc ! videoconvert ! x264enc ! rtph264pay config-interval=10 pt=96 ! udpsink host=127.0.0.1 port=";
				uri << "appsrc ! videoconvert ! x264enc ! rtph264pay config-interval=10 pt=96 ! udpsink host=192.168.1.255 auto-multicast=true port=";
				
			}

			
			REGISTER_P(int, port);

			REGISTER_P(std::string, cmd);

			~StreamResultFilter()
			{

			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

			void StreamVideo()
			{
				
			}

		public:
			HipeStatus computeFPS();

			cv::Size getImageDimension();

			HipeStatus process();

			/*virtual void cleanUp()
			{
				
				
			}*/

			void dispose()
			{
				current_time.tv_sec = current_time.tv_usec = 0;

				nb_frame = 0;
			}
		};
		
		ADD_CLASS(StreamResultFilter, cmd, port);

	}
}
