#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <core/queue/ConcurrentQueue.h>
#include <opencv2/highgui/highgui.hpp>
#include <filter/data/InputData.h>
#include <streaming/Streaming.h/Streaming.h>

namespace filter
{
	namespace algos
	{
		class StreamResultFilter : public IFilter
		{
			core::queue::ConcurrentQueue<filter::data::InputData> queue;

			REGISTER(StreamResultFilter, ())
			{
				

				port = 8864;
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
			HipeStatus process(std::shared_ptr<filter::data::IOData>& outputData)
			{
				core::queue::ConcurrentQueue<data::IOData>* concurrent_queue = Streaming::getInstance()->getStreaming(port);

				
				if (concurrent_queue->hasListener())
				{
					filter::data::IOData copy(_data, true);
					concurrent_queue->push(copy);
				}


				return OK;
			}

			/*virtual void cleanUp()
			{
				
			}*/

			void dispose()
			{
				
			
			}
		};

		ADD_CLASS(StreamResultFilter, port);
	}
}
