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

#include <algos/extraction/ExtractShape.h>


data::ImageArrayData filter::algos::ExtractShape::extractMask(data::ImageData& img, data::ShapeData& datas)
{
	data::ImageArrayData arrays;

	cv::Mat mask = cv::Mat::zeros(img.getMat().size(), CV_8UC1);
	cv::Mat gray = cv::Mat::zeros(img.getMat().size(), CV_8UC1);
	cv::cvtColor(img.getMat(), gray, CV_BGR2GRAY);
	//
	//Get rectangle from list of Point2f
	//

	cv::Rect rectPoint2f;
	rectPoint2f.x = std::numeric_limits<int>::max();
	rectPoint2f.y = std::numeric_limits<int>::max();
	cv::Point pt_max;
	pt_max.x = 0;
	pt_max.y = 0;
	if (! datas.PointsArray().empty())
	{
		fillConvexPoly(mask, datas.PointsArray(), datas.PointsArray().size());
	}

	if (! datas.FreeshapeArray().empty())
	{
		// find the contours

		//cv::findContours(gray, datas.FreeshapeArray(), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);


		// CV_FILLED fills the connected components found
		drawContours(mask, datas.FreeshapeArray(), -1, cv::Scalar(255), CV_FILLED);
	}


	for (cv::Rect rect : datas.RectsArray())
	{
		if (rect.x < 0 || rect.y < 0)
			continue;

		if ((rect.x + rect.width ) >= mask.size().width ||
			(rect.y + rect.height)  >= mask.size().height)
			
			continue;
		cv::rectangle(mask, rect, cv::Scalar(255), CV_FILLED);
		cv::Mat res(img.getMat().clone(), rect);
		arrays.Array().push_back(res);
		saved = res;
	}

	for (cv::Vec3f vec3_f : datas.CirclesArray())
	{
		cv::circle(mask, cv::Point(vec3_f[0], vec3_f[1]), vec3_f[2], cv::Scalar(255), CV_FILLED);
	}

	//TODO Get the list of crop
	cv::Mat result;
	/*if (arrays.Array().empty())
	{
		if (! saved.empty())
			arrays.Array().push_back(saved);
	}*/
	//cv::cvtColor(mask, result, CV_GRAY2BGR);
	//img.getMat().copyTo(result, mask);

	//arrays.Array().push_back(result);

	return arrays;
}

data::ImageData filter::algos::ExtractShape::extractSourceImageData(data::Data& data)
{
	const data::IODataType& type = data.getType();
	if (type == data::IMGF)
	{
		return static_cast<data::ImageData &>(data);
	}
	else if (type == data::PATTERN)
	{
		data::PatternData pattern = static_cast<data::PatternData &>(data);
		return pattern.imageRequest();
	}
		// Now we can extract source image directly from dirpatterndata
	else if (type == data::DIRPATTERN)
	{
		data::DirPatternData& dirPattern = static_cast<data::DirPatternData &>(data);
		return dirPattern.imageSource();
	}
	else
	{
		return data::ImageData();
	}
}

bool filter::algos::ExtractShape::isDrawableSource(const data::Data& data)
{
	switch (data.getType())
	{
	case data::IMGF:
	case data::PATTERN:
	case data::DIRPATTERN:
		return true;
	default:
		return false;
	}
}

bool filter::algos::ExtractShape::isOverlayData(const data::Data& data)
{
	switch (data.getType())
	{
	case data::SHAPE:
		return true;
	default:
		return false;
	}
	return false;
}

HipeStatus filter::algos::ExtractShape::process()
{
	if (_connexData.empty())
		return OK;

	// Separate shapes from source image
	std::vector<data::ShapeData> shapes;
	data::ImageData image;
	bool isSourceFound = false;

	while (!_connexData.empty())
	{
		data::Data data = _connexData.pop();
		if (isOverlayData(data))
		{
			shapes.push_back(static_cast<data::ShapeData &>(data));
		}
		else if (isDrawableSource(data))
		{
			if (isSourceFound)
				throw HipeException(
					"Error in OverlayFilter: Overlay filter only accepts one input source to draw on. Other input data should only be ShapeData objects.");
			isSourceFound = true;

			image = extractSourceImageData(data);
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "Error in ExtractShape: Input type is not handled: ";
			errorMessage << data::DataTypeMapper::getStringFromType(data.getType());
			errorMessage << std::endl;
			throw HipeException(errorMessage.str());
		}
	}

	if (! isSourceFound)
	{
		return OK;
	}

	for (data::ShapeData shape : shapes)
	{
		data::ImageArrayData image_array_data = extractMask(image, shape);
		if (image_array_data.empty())
		{
			PUSH_DATA(data::ImageData());
			
		}
		else
		{
			PUSH_DATA(data::ImageData(image_array_data.Array()[0]));
		}
	}

	return OK;
}
 
