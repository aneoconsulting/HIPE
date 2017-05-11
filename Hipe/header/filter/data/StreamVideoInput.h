#pragma once


#pragma once

#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include <filter/data/VideoData.h>
#include <streaming/CaptureVideo.h>
#include <filter/filter_export.h>
#include "ImageData.h"


namespace filter
{
	namespace data
	{
		class FILTER_EXPORT StreamVideoInput : public VideoData<StreamVideoInput>
		{
			boost::filesystem::path _filePath;
			std::shared_ptr<CaptureVideo> _capture;
			
			cv::Mat asOutput() const;

		private:
			StreamVideoInput();

		public:
			using VideoData<StreamVideoInput>::VideoData;


			StreamVideoInput(const StreamVideoInput &data);


			StreamVideoInput(const std::string & url);

			virtual ~StreamVideoInput();

			Data newFrame();

			bool empty() const
			{
				cv::Mat data;
				return This_const()._capture.get()->read(data) != OK;;
			}
		};
	}
}
