#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/tools/RegisterTable.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <core/queue/ConcurrentQueue.h>
#include <opencv2/highgui/highgui.hpp>
#include <filter/data/InputData.h>
#include <streaming/Streaming.h>
#include <filter/tools/filterMacros.h>
#include <filter/data/InputData.h>


namespace filter
{
	namespace algos
	{
		class StreamResultFilter : public IFilter
		{
			core::queue::ConcurrentQueue<filter::data::InputData> queue;
			struct timeval current_time;
			int fps_avg;
			int nb_frame;

			REGISTER(StreamResultFilter, ())
			{
				port = 8864;
				current_time.tv_sec = 0;
				current_time.tv_usec = 0;
				fps_avg = 0;
				nb_frame = 0;
			}

			REGISTER_P(int, port);

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

			HipeStatus process(std::shared_ptr<filter::data::IOData>& outputData);

			/*virtual void cleanUp()
			{
				
				
			}*/

			void dispose()
			{
				current_time.tv_sec = current_time.tv_usec = 0;

				nb_frame = 0;
			}
		};
		
		ADD_CLASS(StreamResultFilter, port);

	}
}
