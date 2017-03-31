#include <filter/Algos/StreamResultFilter.h>


namespace filter
{
	namespace algos
	{
		HipeStatus StreamResultFilter::process(std::shared_ptr<filter::data::IOData>& outputData)
		{
			core::queue::ConcurrentQueue<data::IOData>* concurrent_queue = Streaming::getInstance()->getStreaming(port);
			
			
			if (concurrent_queue->hasListener())
			{
				filter::data::IOData copy(_data, true);
				concurrent_queue->push(copy);
			}
			

			return OK;
		}
	}
}
