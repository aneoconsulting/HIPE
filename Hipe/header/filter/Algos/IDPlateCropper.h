#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <filter/data/ImageData.h>

#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudaimgproc.hpp>

#include "filter/Algos/IDPlate/IDPlateTools.h"


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
				_debug = 0;
			}
			REGISTER_P(bool, useGPU);
			REGISTER_P(int, bfilterPasses);
			REGISTER_P(int, _debug);

		public:
			HipeStatus process() override;

		private:
			/**
			 * \brief Preprocess plate image and binarize it then search for blobs. The biggest one will be the ROI with the text
			 * \param plateImage the ID plate photo in color
			 * \return an extracted ROI containing only the plate and its text
			 */
			cv::Mat processPlateImage(const cv::Mat& plateImage);


			/**
			 * \brief Find and mask all the blobs of and image
			 * \param plateImageBlackWhite A binary input image
			 * \param color The color used to mask all found and processed blobs
			 * \return The position of the biggest blob in the image
			 */
			cv::Point maskBlobs(cv::Mat& plateImageBlackWhite, const cv::Scalar& color);
		};

		ADD_CLASS(IDPlateCropper, useGPU, bfilterPasses, _debug);
	}
}
