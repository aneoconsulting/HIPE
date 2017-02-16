#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <core/HipeException.h>
#include <filter/data/InputData.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>



namespace filter
{
	namespace algos
	{
		class Resize : public filter::IFilter
		{
			REGISTER(Resize, ())
			{

			}

			REGISTER_P(double, ratio);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process(std::shared_ptr<filter::data::IOData> & outputData)
			{
				auto &arrayInputMat = _data.getInputData();
				outputData.reset(&_data, [](filter::data::IOData*){});
				auto &arrayOutputMat = outputData.get()->getInputData();

				for (int i = 0; i < arrayInputMat.size(); i++)
				{
					int width = arrayInputMat[i].cols;
					int height = arrayInputMat[i].rows;
					cv::Size size(width / ratio, height / ratio);
					cv::resize(arrayInputMat[i], arrayOutputMat[i], size, 0.0, 0.0, cv::INTER_CUBIC);
				}
				
				return OK;
			}

		};

		ADD_CLASS(Resize, ratio);
	}
}
