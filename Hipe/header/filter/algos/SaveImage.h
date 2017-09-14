#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

#include <filter/data/ImageData.h>

#include <iomanip>
#include <ctime>
#include <sstream>

namespace filter
{
	namespace algos
	{
		class SaveImage : public IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(SaveImage, ()), _connexData(data::INOUT)
			{
				path = "";
				filename = "";
				extension = "";
			}

			REGISTER_P(std::string, path);
			REGISTER_P(std::string, filename);
			REGISTER_P(std::string, extension);

		public:
			HipeStatus process() override
			{
				if(path.empty() || filename.empty())
				{
					throw HipeException("[ERROR]: SaveImage::process - You must provide a path and a filename");
				}

				data::ImageData data =  _connexData.pop();
				cv::Mat image(data.getMat());

				// Get Date
				struct tm *time;
				time_t currTime = std::time(nullptr);
				time = std::localtime(&currTime);

				std::ostringstream oss;
				oss << std::put_time(time, "%d%m%Y_%H%M%S");
				std::string date = oss.str();

				// Get full filename
				std::string completeFilename = path + date + "_" + filename + "." + extension;

				// Write image
				if(!cv::imwrite(completeFilename, image))
				{
					throw HipeException("[ERROR]: SaveImage::process - Couldn't write image to file. Check path, extension and quality flag.");
				}
				return OK;
			}
		};

		ADD_CLASS(SaveImage, path, filename, extension);
	}
}