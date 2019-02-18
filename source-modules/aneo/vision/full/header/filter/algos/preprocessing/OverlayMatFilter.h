//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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
						PUSH_DATA(data::ImageData(matImg));
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
					PUSH_DATA(data::ImageData(result));
				}
				return OK;
			}
		};

		ADD_CLASS(OverlayMatFilter, ratio);
	}
}
