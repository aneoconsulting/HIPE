//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#pragma warning(pop)

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
