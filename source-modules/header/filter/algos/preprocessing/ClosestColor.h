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
#include <corefilter/IFilter.h>
#include <data/ImageData.h>
#include <core/HipeStatus.h>


namespace filter
{
	namespace algos
	{
		/**
		 * \brief The ClosestColor filter will find the closest known color a pixel have. The input image must only contain 1 pixel.
		 */
		class ClosestColor : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(ClosestColor, ()), _connexData(data::INDATA)
			{
				setColors();
			}
			REGISTER_P(char, unused);

		public:
			HipeStatus process()
			{
				data::ImageData data = _connexData.pop();
				cv::Mat img = data.getMat();

				if (img.rows == 1 && img.cols == 1)
				{
					cv::Vec3b color = img.at<cv::Vec3b>(0, 0);
					Color closestColor = getClosestColor(color);

					cv::Mat result(cv::Size(32, 32), img.type(), closestColor.value);

					PUSH_DATA(data::ImageData(result));
				}
				else
				{
					throw HipeException("ClosestColor filter is WIP and works with ImageData. Send data with only 1 pixel!");
				}

				return OK;
			}

		private:
			/**
			 * \brief The representation of a color
			 */
			struct Color
			{
				cv::Scalar value;	//<! The RBG values of the color
				std::string name;	//<! The name of the color
			};

		private:
			std::vector<Color> _colors;		//<! The list of all referenced colors

		private:
			/**
			 * \brief Computes the euclidean distance between 2 pixels values (colors)
			 * \param query The queried pixel (or pixel a)
			 * \param ref  The reference pixel (or pixel b)
			 * \return The computed distance
			 */
			double euclideanDistance(const cv::Scalar& query, const cv::Scalar& ref)
			{
				double eucl_dist = 0;
				for (int i = 0; i < 3; i++)
					eucl_dist += (query[i] - ref[i])*(query[i] - ref[i]);
				return eucl_dist;
			}

			/**
			 * \brief Find the closest known color to the inputed pixel
			 * \param query The queried pixel
			 * \return The closest found color
			 */
			Color getClosestColor(const cv::Scalar& query)
			{
				Color closestColor = _colors[0];
				double minDelta = euclideanDistance(query, closestColor.value);

				for (auto& color : _colors)
				{
					double delta = euclideanDistance(query, color.value);

					if (delta < minDelta)
					{
						minDelta = delta;
						closestColor = color;
					}
				}

				return closestColor;

				// Imported Code
				//if (closest_color.value != cv::Scalar(127, 127, 127))
				//{
				//	return closest_color;
				//}
				//else //Find shade of gray / black / white
				//{
				//	int avgChannel = (query[0] + query[1] + query[2]) / 3;
				//	if (avgChannel > 200)
				//		return "white";
				//	if (avgChannel > 150)
				//		return "light gray";
				//	if (avgChannel > 90)
				//		return "gray";
				//	if (avgChannel> 30)
				//		return "dark gray";
				//	else
				//		return "black";
				//}
			}

			/**
			 * \brief Populate the list of known colors. The known colors are the one we're trying to match others to.
			 */
			void setColors()
			{
				// Known colors must be set here
				_colors.clear();

				Color color;

				color.name = "dark blue";
				color.value = cv::Scalar(127, 0, 0);
				_colors.push_back(color);

				color.name = "blue";
				color.value = cv::Scalar(255, 0, 0);
				_colors.push_back(color);

				color.name = "light blue";
				color.value = cv::Scalar(255, 127, 0);
				_colors.push_back(color);

				color.name = "cyan";
				color.value = cv::Scalar(255, 255, 0);
				_colors.push_back(color);

				color.name = "dark green";
				color.value = cv::Scalar(0, 127, 0);
				_colors.push_back(color);

				color.name = "green";
				color.value = cv::Scalar(0, 255, 0);
				_colors.push_back(color);

				color.name = "light green";
				color.value = cv::Scalar(127, 255, 0);
				_colors.push_back(color);

				color.name = "dark red";
				color.value = cv::Scalar(0, 0, 127);
				_colors.push_back(color);

				color.name = "red";
				color.value = cv::Scalar(0, 0, 255);
				_colors.push_back(color);

				color.name = "orange";
				color.value = cv::Scalar(0, 127, 255);
				_colors.push_back(color);

				color.name = "yellow";
				color.value = cv::Scalar(0, 255, 255);
				_colors.push_back(color);

				color.name = "purple";
				color.value = cv::Scalar(127, 0, 127);
				_colors.push_back(color);

				color.name = "magenta";
				color.value = cv::Scalar(255, 0, 255);
				_colors.push_back(color);

				color.name = "black";
				color.value = cv::Scalar(0, 0, 0);
				_colors.push_back(color);

				color.name = "dark gray";
				color.value = cv::Scalar(63, 63, 63);
				_colors.push_back(color);

				color.name = "grey";
				color.value = cv::Scalar(127, 127, 127);
				_colors.push_back(color);

				color.name = "light gray";
				color.value = cv::Scalar(190, 190, 190);
				_colors.push_back(color);

				color.name = "white";
				color.value = cv::Scalar(255, 255, 255);
				_colors.push_back(color);
			}
		};

		ADD_CLASS(ClosestColor, unused);
	}
}

