#include <filter/algos/FilePatternFilter.h>


namespace filter
{
	namespace algos
	{
		HipeStatus FilePatternFilter::process()
		{
			auto p = _connexData.pop();
			auto pathdir = static_cast<data::DirectoryImgData>(p.DirectoryImg()).DirectoryPath();
			auto fullPath = pathdir.append("\\").append(filePath);
			data::ImageData imageInput = p.imageSource();
			cv::Mat imageLoaded = cv::imread(fullPath, CV_LOAD_IMAGE_ANYCOLOR);
			if(imageLoaded.data)
			{
				std::vector<int> crop = { 0,0, imageLoaded.size().width, imageLoaded.size().height };
				data::ImageData imagedata(imageLoaded);
				data::SquareCrop squarecrop;
				squarecrop << imagedata;
				squarecrop << crop;
				data::PatternData pattern(imageInput, squarecrop);
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
