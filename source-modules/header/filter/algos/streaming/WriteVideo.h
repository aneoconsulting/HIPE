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
#include "algos/show/WriteText.h"

#pragma warning(pop)

namespace data {
	class ImageData;
}

namespace filter
{
	namespace algos
	{
		class WriteVideo : public filter::IFilter
		{
			SET_NAMESPACE("vision/utils")

				CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(WriteVideo, ()), _connexData(data::INDATA)
			{
				_debug = 0;
				skip_frame = 0;
				id = -1;
				count = 0;
				dir_path = "videos";
				fps_avg = 25;
				url = "";
			}

			int id;
			int count;
			cv::VideoWriter writer;

			REGISTER_P(int, _debug);

			REGISTER_P(int, skip_frame);

			REGISTER_P(int, fps_avg);

			REGISTER_P(std::string, dir_path);

			REGISTER_P(std::string, prefix_filename);
			REGISTER_P(std::string, url);

			

			int getLastKnownIDFromDirectory();

			HipeStatus process() override;


			void dispose()
			{
				if (writer.isOpened())
				{
					writer.release();
				}
			}
		};

		ADD_CLASS(WriteVideo, _debug, skip_frame, fps_avg, dir_path, prefix_filename, url);


	}
}