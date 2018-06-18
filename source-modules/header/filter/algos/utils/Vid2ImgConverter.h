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

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class Vid2ImgConverter : public filter::IFilter
		{
			SET_NAMESPACE("vision/utils")

			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(Vid2ImgConverter, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				skip_frame = 0;
				id = -1;
				count = 0;
				dir_path = "images";
			}

			int id;
			int count;

			REGISTER_P(int, _debug);

			REGISTER_P(int, skip_frame);
			
			REGISTER_P(std::string, dir_path);

			REGISTER_P(std::string, prefix_filename);

			int getLastKnownIDFromDirectory();

			HipeStatus savePaternPicture(const cv::Mat& picture);

			HipeStatus process() override;

		};

		ADD_CLASS(Vid2ImgConverter, _debug, skip_frame, dir_path, prefix_filename);


	}
}