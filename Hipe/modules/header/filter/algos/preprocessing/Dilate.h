//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class Dilate : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Dilate, ()), _connexData(data::INDATA)
			{
				iterations = 1;
				kernelSizeX = 1;
				kernelSizeY = 1;
				morphType = "DILATE";
				morphShape = "RECT";
				anchorX = -1;
				anchorY = -1;
			}
			REGISTER_P(int, iterations);
			REGISTER_P(std::string, morphType);
			REGISTER_P(std::string, morphShape);
			REGISTER_P(int, kernelSizeX);
			REGISTER_P(int, kernelSizeY);
			REGISTER_P(int, anchorX);
			REGISTER_P(int, anchorY);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] Dilate::process - No input data found.");
				}

				std::transform(morphType.begin(), morphType.end(), morphType.begin(), ::toupper);
				std::transform(morphShape.begin(), morphShape.end(), morphShape.begin(), ::toupper);

				int type = convertMorphType(morphType);
				int shape = convertMorphShape(morphShape);

				cv::Point anchor(anchorX, anchorY);

				cv::Mat output;
				cv::Mat dilateKernel = cv::getStructuringElement(shape, cv::Size(kernelSizeX, kernelSizeY), anchor);
				cv::dilate(image, output, dilateKernel, anchor, iterations);

				PUSH_DATA(data::ImageData(output));

				return OK;
			}

		private:
			int convertMorphType(const std::string& name);
			int convertMorphShape(const std::string& name);
		};
		ADD_CLASS(Dilate, iterations, morphType, morphShape, kernelSizeX, kernelSizeY, anchorX, anchorY);
	}
}