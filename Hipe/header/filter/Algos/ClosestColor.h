#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <filter/data/ImageData.h>
#include <core/HipeStatus.h>

#include <opencv2/core.hpp>

namespace filter
{
	namespace algos
	{
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

					_connexData.push(data::ImageData(result));
				}
				else
				{
					throw HipeException("ClosestColor filter is WIP and works with ImageData. Send data with only 1 pixel!");
				}

				return OK;
			}

		private:
			struct Color
			{
				cv::Scalar value;
				std::string name;
			};

		private:
			std::vector<Color> _colors;

		private:
			double euclideanDistance(const cv::Scalar& query, const cv::Scalar& ref)
			{
				double eucl_dist = 0;
				for (int i = 0; i < 3; i++)
					eucl_dist += (query[i] - ref[i])*(query[i] - ref[i]);
				return eucl_dist;
			}

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

			void setColors()
			{
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

