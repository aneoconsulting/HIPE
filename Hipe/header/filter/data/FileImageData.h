#pragma once

#include <filter/data/ImageData.h>
#include <filter/data/IODataType.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>

namespace filter
{
	namespace data
	{
		class FileImageData : public IOData<ImageData, FileImageData>
		{
			boost::filesystem::path _filePath;

			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }
		private:
			FileImageData() : IOData(IODataType::IMGF)
			{
				
			}

		public:

			FileImageData(const FileImageData & right) : IOData(IODataType::IMGF)
			{
				Data::registerInstance(right._This);
			}

			FileImageData(const std::string & filePath) : IOData(IODataType::IMGF)
			{
				Data::registerInstance(new FileImageData());
				This()._filePath = filePath;
				This()._type = IMGF;

				cv::Mat mat = cv::imread(filePath, CV_LOAD_IMAGE_COLOR);
				if (mat.empty())
				{
					std::stringstream strbuild;
					strbuild  << "Cannot open file : " << filePath;

					throw HipeException(strbuild.str());
				}
				This()._array.push_back(mat);
				
			}

			virtual void copyTo(ImageData& left) const
			{
				ImageData::copyTo(static_cast<ImageData &>(left));

			}

			FileImageData& operator=(const FileImageData& left)
			{
				_This = left._This;
				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}

		};
	}
}
