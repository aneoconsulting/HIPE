#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/PatternData.h>

namespace filter
{
	namespace algos
	{
		class ExtractPatternSource : public IFilter
		{
			CONNECTOR(data::PatternData, data::ImageData);

			REGISTER(ExtractPatternSource, ()), _connexData(data::INDATA)
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

					data::ImageData output(pattern.imageRequest().getMat().clone());

					// Copy data to match INDATA intended mode
					_connexData.push(output);
				}

				return OK;
			}
		};

		ADD_CLASS(ExtractPatternSource, unused);
	}
}
