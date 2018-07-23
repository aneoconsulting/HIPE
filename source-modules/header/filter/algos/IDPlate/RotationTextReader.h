//@HIPE_LICENSE@
#pragma once
#include <corefilter/Model.h>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>

#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>	
#include "data/VideoData.h"
#include "data/ShapeData.h"

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class RotationTextReader : public filter::IFilter
		{
			SET_NAMESPACE("vision/Text")

			CONNECTOR(data::Data, data::ImageData);

			REGISTER(RotationTextReader, ()), _connexData(data::INDATA)
			{
				_debug = 0;

			}
		
			REGISTER_P(int, _debug);

			bool isDrawableSource(const data::Data& data);

			bool isShapeData(const data::Data& data);

			data::ImageData extractSourceImageData(data::Data& data);

			cv::Mat getDiscFromCirlce(const cv::Mat& image, cv::Vec<float, 3> circle);

			cv::Mat transformInBlackAndWhite(const cv::Mat& image);

			std::vector<cv::Mat> generateRotationText(const cv::Mat& image, int offset_rotation);

			std::vector<std::string> readText(const std::vector<cv::Mat>& mats);

			HipeStatus process() override;


			/**
			* \brief Be sure to call the dispose method before to destroy the object PushGraphToNode
			*/
			virtual void dispose()
			{
			
			}
		};

		ADD_CLASS(RotationTextReader, _debug);


	}
}