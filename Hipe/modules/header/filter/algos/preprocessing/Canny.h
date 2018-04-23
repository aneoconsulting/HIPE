//@HIPE_LICENSE@
#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		/**
		 *  \var Canny::blurKernel
		 *  The size of the blur kernel to use. \see cv::blur()
		 *
		 *  \var Canny::thresh1
		 *  The first threshold for the canny's hysteresis procedure.
		 *
		 *  \var Canny::thresh2
		 *  The second threshold for the canny's hysteresis procedure.
		 *
		 *  \var Canny::aperture
		 *  The aperture size for the sobel operator. \see cv::Sobel()
		 */

		/**
		 * \brief The Canny filter will find the edges of the objects in an image.
		 *
		 *  The image needs to be in grayscale. If not it will be converted at runtime.
		 *  The filter will first blur the image (for better results), then use the canny algorithm.
		 *  The output of the canny algorithm will help to find the contours with the OpenCV findContours function.
		 *  \see cv::Canny()
		 *  \see cv::findContours()
		 */
		class Canny : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Canny, ()), _connexData(data::INDATA)
			{
				blurKernel = 0;
				aperture = 3;
			}

			REGISTER_P(int, blurKernel);
			REGISTER_P(double, thresh1);
			REGISTER_P(double, thresh2);
			REGISTER_P(int, aperture);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat srcImg = data.getMat();
				if (!srcImg.data)
				{
					throw HipeException("[Error] Canny::process - No input data found.");
				}

				cv::Mat gsImg, canny;
				std::vector<std::vector<cv::Point>> contours;
				std::vector<cv::Vec4i> hierarchy;

				// Canny must be applied on a grayscale image. We suppose either the image is in color (3 or 4 channels) or already in grayscale (1 channel)
				const int channels = srcImg.channels();
				if (channels == 3 || channels == 4)
				{
					cv::cvtColor(srcImg, gsImg, CV_BGR2GRAY);
				}
				// [TODO] Does 1 channel always means grayscale ? Handle 2 channels images (grayscale + alpha) ?
				else if (channels == 1)
				{
					std::cout << "[LOG] Canny::process - Source image is already in grayscale, image will not be converted again.";
					gsImg = srcImg.clone();
				}
				else
				{
					std::stringstream errMessage;
					errMessage << "[ERROR] Canny::process - Source must be converted in grayscale but is in an incorrect color type (has " << srcImg.channels() << " channels, expected 3 or 4).";
					throw HipeException(errMessage.str());
				}

				// Assert kernel size is correct then blur image
				if (blurKernel > 0)
				{
					if (!(blurKernel % 2))
					{
						throw HipeException("[Error] Canny::process - blur kernel needs to be odd sized and of value > 1. Passed value is " + std::to_string(blurKernel));
					}
					else
					{
						cv::Size kernelBlur(blurKernel, blurKernel);
						cv::blur(gsImg, gsImg, kernelBlur);
					}
				}

				// Contours
				cv::Canny(gsImg, canny, thresh1, thresh2, aperture, false);
				cv::findContours(canny, contours, hierarchy, cv::RetrievalModes::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

				// Draw contours
				cv::Scalar color(255, 255, 255);
				const int contourThickness = 1;
				const cv::LineTypes lineType = cv::LINE_8;

				cv::Mat contoursImage = cv::Mat::zeros(srcImg.size(), CV_8UC1);
				for (size_t i = 0; i < contours.size(); ++i)
				{
					cv::drawContours(contoursImage, contours, static_cast<int>(i), color, contourThickness, lineType, hierarchy, 0, cv::Point(0, 0));
				}

				PUSH_DATA(data::ImageData(contoursImage));

				return OK;
			}
		};

		ADD_CLASS(Canny, blurKernel, thresh1, thresh2, aperture) ;
	};
};
