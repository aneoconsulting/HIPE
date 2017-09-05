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
#include <string>


namespace filter
{
	namespace algos
	{
		/**
		 * [TODO]
		 * \brief The StreamResultFilter filter is used to stream the result of a graph to a distant target.
		 */
		class StreamResultFilter : public IFilter
		{
			//data::ConnexOutput<filter::data::ImageData> connData;
			CONNECTOR_OUT(data::ImageData);

			core::queue::ConcurrentQueue<filter::data::ImageData> queue;	//<! [TODO] unused ?
			struct timeval current_time;	//<! The current time
			int fps_avg;	//<! [TODO] The average FPS at which the video is played
			int nb_frame;	//<! [TODO]
			

			// second part of sender pipeline
			std::stringstream uri;	//<! The uri of the device on which the video will be streamed
			cv::VideoWriter writer;	//<! [TODO]

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
				uri << "appsrc ! videoconvert ! x264enc ! rtph264pay config-interval=10 pt=96 ! udpsink host=192.168.1.255 auto-multicast=true port=";
				
			}

			
			REGISTER_P(int, port);			//<! The port on which the target device is listening

			REGISTER_P(std::string, cmd);	//<! The command line containing all the streaming parameters

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
			/**
			 * \brief [TODO] Computes the average fps at which the video is played
			 * \return [TODO]
			 */
			HipeStatus computeFPS();

			/**
			 * \brief Get the dimensions of the video
			 * \return Returns the dimensions of the video as a cv::Size object
			 */
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
