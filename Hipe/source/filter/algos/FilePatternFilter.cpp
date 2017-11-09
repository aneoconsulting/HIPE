#pragma once
#include <filter/algos/FilePatternFilter.h>


namespace filter
{
	namespace algos
	{
		HipeStatus FilePatternFilter::process()
		{
			auto p = _connexData.pop();
			auto pathdir = static_cast<data::DirectoryImgData>(p.DirectoryImg()).DirectoryPath();
			auto fullPath = pathdir.append(filePath);
		
			cv::Mat crop = cv::imread(fullPath, CV_LOAD_IMAGE_ANYCOLOR);
			if(crop.data)
			{
				data::ImageData imagedata(crop);
				data::SquareCrop squarecrop(imagedata);
				data::PatternData pattern(squarecrop);
				_connexData.push(pattern);
			}
			else
			{
				return DATA_EMPTY;
			}
			
			return OK;
		}
	}
}
