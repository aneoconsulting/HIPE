#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <core/HipeException.h>
#include <filter/data/InputData.h>
#include <Core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

namespace filter
{
	namespace algos
	{
		class ShowImage : public filter::IFilter
		{
			REGISTER(ShowImage, ())
			{

			}

			REGISTER_P(int, sigma);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				throw HipeException(_constructor + " process isn't yet implmented");
			}

			HipeStatus process(data::IOData & InputData, data::IOData outputData)
			{
				
				::cv::imshow(_name, InputData.getInputData(0));
				char c;
				std::cout << "Waiting to finish" << std::endl;
				cv::waitKey(0);

				

				::cv::destroyWindow(_name);


				return OK;
			}

		};

		ADD_CLASS(ShowImage, sigma);
	}
}
