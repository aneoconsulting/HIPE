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

			~ShowImage()
			{
				cv::destroyWindow(_name);
			}

			REGISTER_P(int, waitkey);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process(std::shared_ptr<filter::data::IOData> & outputData)
			{
				cv::namedWindow(_name);
				::cv::imshow(_name, _data.getInputData(0));
				char c;
				std::cout << "Waiting to finish" << std::endl;
				cv::waitKey(0);

				

				


				return OK;
			}

			void dispose()
			{
				Model::dispose();
				cv::destroyWindow(_name);
			}

		};

		ADD_CLASS(ShowImage, waitkey);
	}
}
