#pragma once

#include <filter/data/ImageData.h>
#include <filter/data/IODataType.h>

namespace filter
{
	namespace data
	{
		class FileImageData : filter::data::ImageData
		{
			std::string _filePath;
			IODataType _type = IODataType::IMG;


		};
	}
}
