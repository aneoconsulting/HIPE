#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/PatternData.h>

namespace filter
{
	namespace algos
	{
		class ExtractPatternCrops : public IFilter
		{
			CONNECTOR(data::PatternData, data::ImageArrayData);

			REGISTER(ExtractPatternCrops, ()), _connexData(data::INDATA)
			{

			}

			REGISTER_P(char, unused);


		public:
			virtual std::string resultAsString() { return std::string("TODO"); };


			HipeStatus process()
			{
				while (!_connexData.empty()) // While i've parent data
				{
					data::PatternData pattern = _connexData.pop();

					if (pattern.getType() != data::IODataType::PATTERN)
						throw HipeException("Error in ExtractPatternCrops: wrong input data type. Expected PATTERN, received " + pattern.getType());

					std::vector<cv::Mat> crops = pattern.patterns();

					data::ImageArrayData output;
					for (auto & crop : crops)
					{
						// Copy data to match INDATA intended mode
						output << crop.clone();
					}

					_connexData.push(output);
				}

				return OK;
			}
		};

		ADD_CLASS(ExtractPatternCrops, unused);
	}
}
