#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

namespace filter
{
	namespace algos
	{
		class ShowVideo : public IFilter
		{
			REGISTER(ShowVideo, ())
			{
				
				waitkey = 30;
			}

			REGISTER_P(int, waitkey);
			~ShowVideo()
			{
				
			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		public:
			HipeStatus process(std::shared_ptr<filter::data::IOData>& outputData)
			{
				cv::Mat input_data = _data.getInputData(0);
				if (input_data.rows <= 0 || input_data.cols <= 0)
					throw HipeException("Image to show doesn't data");
				::cv::imshow(_name, input_data);
				cvWaitKey(10);		
				
				return OK;
			}

			void dispose()
			{
				Model::dispose();
				cv::destroyWindow(_name);
			}
		};

		ADD_CLASS(ShowVideo, waitkey);
	}
}
