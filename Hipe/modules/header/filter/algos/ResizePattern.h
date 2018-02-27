#pragma once


#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/PatternData.h>


namespace filter
{
	namespace algos
	{
		/**
		 * \var ResizePattern::ratio
		 * The factor by which the image must be rescaled.
		 */

		/**
		 * \brief The ResizePattern filter is the Resize filter version adapted to PatternData objects.
		 */
		class ResizePattern : public filter::IFilter
		{
			CONNECTOR(data::PatternData, data::PatternData);

			REGISTER(ResizePattern, ()), _connexData(data::INOUT)
			{

			}

			REGISTER_P(double, ratio);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				while (!_connexData.empty()) // While i've parent data
				{
					data::PatternData pattern = _connexData.pop();
					if (pattern.getType() != data::PATTERN)
					{
						throw HipeException("The resizePattern only accepts PatternData");
					}

					{
						int width = pattern.imageRequest().getMat().cols;
						int height = pattern.imageRequest().getMat().rows;
						cv::Size size(width / ratio, height / ratio);
						cv::Mat res;
						cv::resize(pattern.imageRequest().getMat(), res, size, 0.0, 0.0, cv::INTER_CUBIC);
						pattern << res;

					}

					{
						data::SquareCrop square_crop = pattern.getSquareCrop();
						int width = square_crop.getPicture().getMat().cols;
						int height = square_crop.getPicture().getMat().rows;
						cv::Size size(width / ratio, height / ratio);
						cv::Mat res;
						cv::resize(square_crop.getPicture().getMat(), res, size, 0.0, 0.0, cv::INTER_CUBIC);
						square_crop << res;

						std::vector<cv::Rect> rects;
						for (auto& value : square_crop.getSquareCrop())
						{
							value.x /= ratio;
							value.y /= ratio;
							value.width /= ratio;
							value.height /= ratio;
							rects.push_back(value);
						}
						square_crop << rects;
						square_crop.crops(true);
					}

				}
				return OK;
			}

		};

		ADD_CLASS(ResizePattern, ratio);
	}
}

