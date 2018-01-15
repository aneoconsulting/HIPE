#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

#include <data/ImageData.h>

#include <iomanip>
#include <ctime>
#include <sstream>

namespace filter
{
	namespace algos
	{
		class OutputImage : public IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);

			REGISTER(OutputImage, ()), _connexData(data::INOUT)
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
				if (path.empty() || filename.empty())
				{
					throw HipeException("[ERROR]: SaveImage::process - You must provide a path and a filename");
				}

				data::ImageData data = _connexData.pop();
				cv::Mat image(data.getMat());

				// Get Date
				struct tm *time;
				time_t currTime = std::time(nullptr);
				time = std::localtime(&currTime);

				// Full filename
				std::ostringstream completeFilename;
				completeFilename << path << std::put_time(time, "%d%m%Y_%H%M%S") << "_" << filename << "." << extension;


				// Write image
				if (!cv::imwrite(completeFilename.str(), image))
				{
					std::stringstream errorMessage;
					errorMessage << "ERROR in OutputImage filter: Couldn't write image to file. Check path, extension and quality flag." << std::endl;
					errorMessage << "Complete filepath is: " << completeFilename.str() << std::endl;
					throw HipeException(errorMessage.str());
				}
				return OK;
			}
		};

		ADD_CLASS(OutputImage, path, filename, extension);
	}
}