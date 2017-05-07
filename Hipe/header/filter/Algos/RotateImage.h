#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>
#include <filter/data/ConnexData.h>
#include <filter/data/ImageData.h>

namespace filter
{
	namespace algos
	{
		class RotateImage : public IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(RotateImage, ()), _connexData(data::INOUT)
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
			HipeStatus process()
			{
				while (!_connexData.empty()) // While i've parent data
				{
					data::ImageArrayData images = _connexData.pop();


					//Resize all images coming from the same parent
					for (auto &myImage : images.Array())
					{
						cv::Point2f src_center(myImage.cols / 2.0F, myImage.rows / 2.0F);
						cv::Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);

						warpAffine(myImage, myImage, rot_mat, myImage.size());
					}
					//No push it's an inout data
				}

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
