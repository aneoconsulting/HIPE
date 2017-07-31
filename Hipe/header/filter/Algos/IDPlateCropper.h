#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <filter/data/ImageData.h>

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudaimgproc.hpp>


namespace filter
{
	namespace algos
	{
		class IDPlateCropper : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(IDPlateCropper, ()), _connexData(data::INDATA)
			{
				useGPU = false;
				bfilterPasses = 2;
				debug_ = false;
			}
			REGISTER_P(bool, useGPU);
			REGISTER_P(int, bfilterPasses);
			REGISTER_P(bool, debug_);

		public:
			HipeStatus process() override;

		private:
			cv::Mat preprocessPlate(const cv::Mat& plateImage);

			cv::Mat applyBilateralFiltering(const cv::Mat& plateImage, int iterations, bool useGPU = false);
			cv::Mat convertToGrayscale(const cv::Mat& plateImageColor);
			cv::Point maskBlobs(cv::Mat& plateImageBlackWhite, const cv::Scalar& color);


			void showImage(const cv::Mat& image);
		};

		ADD_CLASS(IDPlateCropper, useGPU, bfilterPasses, debug_);
	}
}
