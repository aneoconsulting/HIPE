#pragma once


#pragma once

#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include <filter/data/VideoData.h>
#include <streaming/CaptureVideo.h>
#include <filter/filter_export.h>


namespace filter
{
	namespace data
	{
		class FILTER_EXPORT StreamVideoInput : public IOData<VideoData, StreamVideoInput>
		{
			boost::filesystem::path _filePath;
			std::shared_ptr<CaptureVideo> _capture;
			
			cv::Mat asOutput() const;

		private:
			StreamVideoInput();

		public:
			using IOData::IOData;


			StreamVideoInput(const StreamVideoInput &data);


			StreamVideoInput(const std::string & url);

			virtual ~StreamVideoInput();

			cv::Mat newFrame() const;
		};
	}
}
