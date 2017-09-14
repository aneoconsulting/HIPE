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
		/**
		 * \var IDPlateCropper::useGPU
		 * Should the GPU be used to apply the bilateral filtering or not.
		 *
		 * \var IDPlateCropper::bfilterPasses
		 * The desired number of times the bilateral filtering should be applied.
		 *
		 * \var IDPlateCropper::_debug
		 * The desired debug level. The default level is 0 (disabled). A higher value will enable more debug informations.
		 */

		/**
		 * \brief The IDPlateCropper filter will try to find an ID plate in an image and crop it.
		 */
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
			 * \brief Preprocess the plate image by binarizing it then searching for blobs. The biggest one will be the ROI englobing the text area.
			 * \param plateImage The input plate image in color.
			 * \return A cropped color image of the found ROI englobing the text area.
			 */
			cv::Mat processPlateImage(const cv::Mat& plateImage);
		};

		ADD_CLASS(IDPlateCropper, useGPU, bfilterPasses, _debug);
	}
}
