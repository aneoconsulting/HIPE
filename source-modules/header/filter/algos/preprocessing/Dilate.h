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
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class Dilate : public filter::IFilter
		{
			CONNECTOR(data::ImageData, data::ImageData);
			REGISTER(Dilate, ()), _connexData(data::INDATA)
			{
				iterations = 1;
				kernelSizeX = 1;
				kernelSizeY = 1;
				morphType = "DILATE";
				morphShape = "RECT";
				anchorX = -1;
				anchorY = -1;
			}
			REGISTER_P(int, iterations);
			REGISTER_P(std::string, morphType);
			REGISTER_P(std::string, morphShape);
			REGISTER_P(int, kernelSizeX);
			REGISTER_P(int, kernelSizeY);
			REGISTER_P(int, anchorX);
			REGISTER_P(int, anchorY);

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				if (!image.data)
				{
					throw HipeException("[Error] Dilate::process - No input data found.");
				}

				std::transform(morphType.begin(), morphType.end(), morphType.begin(), ::toupper);
				std::transform(morphShape.begin(), morphShape.end(), morphShape.begin(), ::toupper);

				int type = convertMorphType(morphType);
				int shape = convertMorphShape(morphShape);

				cv::Point anchor(anchorX, anchorY);

				cv::Mat output;
				cv::Mat dilateKernel = cv::getStructuringElement(shape, cv::Size(kernelSizeX, kernelSizeY), anchor);
				cv::dilate(image, output, dilateKernel, anchor, iterations);

				PUSH_DATA(data::ImageData(output));

				return OK;
			}

		private:
			int convertMorphType(const std::string& name);
			int convertMorphShape(const std::string& name);
		};
		ADD_CLASS(Dilate, iterations, morphType, morphShape, kernelSizeX, kernelSizeY, anchorX, anchorY);
	}
}
