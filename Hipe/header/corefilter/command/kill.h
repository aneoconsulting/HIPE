#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/PatternData.h>
#include <opencv2/xfeatures2d.hpp>
#include <atomic>


namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT Kill : public filter::IFilter
		{
			std::atomic<bool> isStart;
			boost::thread *thr_server;
			core::queue::ConcurrentQueue<data::PatternData> imagesStack;
			
			CONNECTOR(data::Data, data::Data);

			REGISTER(Kill, ()), _connexData(data::INDATA)
			{
				
				ignore = 2;
			}


			REGISTER_P(int, ignore);




			virtual std::string resultAsString() { return std::string("TODO"); };

		public:

			void startDetectObject();

			HipeStatus process();

			virtual void dispose()
			{
			
			}
		};

		ADD_CLASS(Kill, ignore);
	}
}
