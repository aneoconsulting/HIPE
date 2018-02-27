#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/SquareCrop.h>


namespace filter
{
	namespace algos
	{
		/**
		 * \var OverlayMatFilter::ratio
		 * [TODO]
		 */

		/**
		 *\todo
		 * \brief [TODO]
		 */
		class OverlayMatFilter : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::Data, data::ImageData);

			REGISTER(OverlayMatFilter, ()), _connexData(data::INDATA)
			{

			}

			REGISTER_P(double, ratio);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				if (_connexData.size() % 2 != 0)
				{
					throw HipeException("The Overlay missing or text data. Please be sure to link properly with parent");
				}

				//while (!_connexData.empty()) // While i've parent data
				{
					data::Data data1 = _connexData.pop();
					data::Data data2 = _connexData.pop();

				
					data::ImageData image;
					data::ImageData overlay;
					if (data1.getType() != data::IMGF || data2.getType() != data::IMGF)
					{
						throw HipeException("Missing image to generate overlay");
					}
					if (data1.getType() == data::IMGF)
						image = static_cast<data::ImageData &>(data1);
					if (data2.getType() == data::IMGF)
						overlay = static_cast<data::ImageData &>(data2);

					
					cv::Mat result(image.getMat().size(), image.getMat().type());
					cv::Mat overMat = overlay.getMat();
					cv::Mat matImg = image.getMat();
					if (overMat.empty())
					{
						_connexData.push(data::ImageData(matImg));
						return OK;
					}

					for (int y = 0;y<matImg.rows;y++)
						for (int x = 0;x<matImg.cols;x++)
						{
							//int alpha = ov.at<Vec3b>(y,x)[3];
							if (overMat.at<cv::Vec3b>(y, x)[0] + overMat.at<cv::Vec3b>(y, x)[1] + overMat.at<cv::Vec3b>(y, x)[2] == 0)
							{
								result.at<cv::Vec3b>(y, x)[0] = matImg.at<cv::Vec3b>(y, x)[0];
								result.at<cv::Vec3b>(y, x)[1] = matImg.at<cv::Vec3b>(y, x)[1];
								result.at<cv::Vec3b>(y, x)[2] = matImg.at<cv::Vec3b>(y, x)[2];
							} 
							else
							{
								result.at<cv::Vec3b>(y, x)[0] = overMat.at<cv::Vec3b>(y, x)[0];
								result.at<cv::Vec3b>(y, x)[1] = overMat.at<cv::Vec3b>(y, x)[1];
								result.at<cv::Vec3b>(y, x)[2] = overMat.at<cv::Vec3b>(y, x)[2];
							}
						}
					_connexData.push(data::ImageData(result));
				}
				return OK;
			}
		};

		ADD_CLASS(OverlayMatFilter, ratio);
	}
}
