#include <filter/Algos/StreamResultFilter.h>


namespace filter
{
	namespace algos
	{

		HipeStatus StreamResultFilter::computeFPS()
		{
			unsigned long long elapse;
			if (fps_avg == 0)
			{
				nb_frame++;
				if (current_time.tv_sec == 0 && current_time.tv_usec == 0)
				{
					hipe_gettimeofday(&current_time, nullptr);
					return WAIT_FPS;
				}
				else
				{
					struct timeval nextTime;
					hipe_gettimeofday(&nextTime, nullptr);
					elapse = ((nextTime.tv_sec - current_time.tv_sec) * 1000000L
						+ nextTime.tv_usec) - current_time.tv_usec;

					

					if (elapse > 1000000L)
					{
						double current_fps = static_cast<double>(nb_frame * 1000000L) / elapse;
						nb_frame = 0;
						current_time = nextTime;
						std::cout << " us, FPS : " << current_fps << std::endl;
						fps_avg = current_fps;
						return OK;
					}
					return WAIT_FPS;
				}
			}
			return OK;
		}

		cv::Size StreamResultFilter::getImageDimension()
		{
			cv::Mat first = _data.getInputData()[0];

			return first.size();
		}

		HipeStatus StreamResultFilter::process(std::shared_ptr<filter::data::IOData>& outputData)
		{
			if (computeFPS() == WAIT_FPS)	return OK;
			
			if (_data.getInputData().empty()) return VECTOR_EMPTY;
			
			cv::Size size = getImageDimension();

			std::shared_ptr<TaskContainer> task_container = Streaming::getInstance()->getStreaming(port, size.height, size.width, fps_avg);

			//filter::data::IOData copy(_data, true);
			if (task_container->isActive()) // next iteration the streamer will be ready and active
			{
				
					task_container->onFrameMethod(_data.getInputData()[0]);
				
			}
			_data.getInputData().clear();

			return OK;
		}
	}
}
