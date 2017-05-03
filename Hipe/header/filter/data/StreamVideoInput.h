#pragma once


#pragma once

#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include <filter/data/VideoData.h>
#include <streaming/CaptureVideo.h>


namespace filter
{
	namespace data
	{
		class DLL_PUBLIC StreamVideoInput : public VideoData
		{
			boost::filesystem::path _filePath;
			std::shared_ptr<CaptureVideo> _capture;

			cv::Mat asOutput() const;

		public:

			StreamVideoInput(const std::string & url);
			StreamVideoInput(StreamVideoInput &data);

			~StreamVideoInput()
			{
				
			}

			bool newFrame()
			{
				cv::Mat data;
				HipeStatus hipe_status = _capture.get()->read(data);
				if (hipe_status == UNKOWN_ERROR)
					throw HipeException("Error grabbing frame");
				if (hipe_status == END_OF_STREAM)
					return false;
				_data.push_back(data);
				return true;
			}

		};
	}
}
