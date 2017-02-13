#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <core/HipeException.h>
#include <filter/data/InputData.h>
#include <filter/IFilter.h>
#include <Core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

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
			HipeStatus process(data::IOData & outputData)
			{
				auto &arrayInputMat = _data.getInputData();
				outputData = _data;
				auto &arrayOutputMat = outputData.getInputData();

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
