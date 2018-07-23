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

#include <filter/algos/show/TextLoggerShow.h>
#include <opencv2/videostab/wobble_suppression.hpp>
#include "algos/utils/CVUtils.h"

size_t filter::algos::TextLoggerShow::computeSizeOfAllText()
{
	size_t size = 0;

	for (size_t i = 0; i < texts.size(); i++)
	{
		int baseline = 0;

		cv::Size text_size = cv::getTextSize(texts[i], cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontscale, 2, &baseline);
		size += text_size.height + 10;
	}
	return size;
}

size_t filter::algos::TextLoggerShow::getNextPosition(int index)
{
	size_t size = 0;

	if (index >= texts.size()) index = texts.size();

	for (size_t i = 0; i < index; i++)
	{
		int baseline = 0;

		cv::Size text_size = cv::getTextSize(texts[i], cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontscale, 2, &baseline);
		size += text_size.height + 10;
	}
	return size;
}

HipeStatus filter::algos::TextLoggerShow::process()
{
	std::vector<data::ShapeData> arrayShapeData;

	while (!_connexData.empty())
	{
		data::Data shape = _connexData.pop();

		if (shape.getType() == data::IODataType::SHAPE)
		{
			data::ShapeData & shape_data = static_cast<data::ShapeData&>(shape);
			arrayShapeData.push_back(shape_data);
		}
	}

	cv::Mat textToImage = cv::Mat::zeros(width, height, CV_8UC3);
	
	for (auto &shape : arrayShapeData)
	{
		for (std::string text : shape.IdsArray())
		{
			//Compute size if higher than 480 then pop before
			size_t height_total = computeSizeOfAllText();
			while (height_total >= height)
			{
				if (texts.empty()) break;

				texts.pop_front();

				height_total = computeSizeOfAllText();
			}
			texts.push_back(text);
		}
	
		
	}

	for (int i = 0; i < texts.size(); i++)
	{
		int baseline = 0;
		cv::Scalar color(0, 113, 245); //ANEO COLOR
		size_t hpos = getNextPosition(i);
		cv::Point position = cv::Point(10, hpos);

		cv::putText(textToImage, texts[i], position,
			cv::HersheyFonts::FONT_HERSHEY_PLAIN, fontscale, color, 2);
	}

	data::ImageData img = data::ImageData(textToImage);

	img.setLabel(this->getName() + "__dataOut");
	_connexData.push(img);

	//PUSH_DATA(data::ImageData(textToImage));



	return OK;
}
