//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <coredata/ConnexData.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		/**
		 * \var RotateImage::angle
		 * The angle (in degrees) to rotate the image by.
		 */

		/**
		 * \brief The RotateImage filter is used to rotate images around the positive Z axis and taking their center as pivot.
		 */
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
