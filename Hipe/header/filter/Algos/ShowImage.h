#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

namespace filter
{
	namespace algos
	{
		class ShowImage : public IFilter
		{
			REGISTER(ShowImage, ())
			{

			}

			REGISTER_P(int, sigma);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process(data::IOData & outputData)
			{
				
				::cv::imshow(_name, _data.getInputData(0));
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
