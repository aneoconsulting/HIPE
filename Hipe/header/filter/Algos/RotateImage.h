#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

namespace filter
{
	namespace algos
	{
		class RotateImage : public IFilter
		{
			REGISTER(RotateImage, ())
			{
				
				angle = 10;
			}

			REGISTER_P(double, angle);

			~RotateImage()
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
				cv::Point2f src_center(input_data.cols / 2.0F, input_data.rows / 2.0F);
				cv::Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
				
				warpAffine(input_data, input_data, rot_mat, input_data.size());

				outputData.reset(&_data, [](filter::data::IOData*){});

				return OK;
			}

			void dispose()
			{
				Model::dispose();
				cv::destroyWindow(_name);
			}
		};

		ADD_CLASS(RotateImage, angle);
	}
}
