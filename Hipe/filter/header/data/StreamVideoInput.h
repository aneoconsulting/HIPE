#pragma once

#include <filter/data/IODataType.h>
#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include "data/VideoData.h"

namespace boost {namespace asio {
	class io_service;
}
}

namespace filter
{
	namespace data
	{
		class StreamVideoInput : public VideoData
		{
			std::string _streamUrl;
			cv::VideoCapture _capture;
			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }

		public:

			StreamVideoInput(const std::string & streamUrl) : VideoData(IODataType::STRMVID)
			{
				_streamUrl = streamUrl;
			}

			StreamVideoInput(StreamVideoInput &data) : VideoData(data)
			{
				_streamUrl = data._streamUrl;
			}

			bool newFrame()
			{
				if (!_capture.isOpened())
				{
					_capture.open(_streamUrl);

					if (!_capture.isOpened())
					{
						std::stringstream str;
						str << "Cannot open video : " << _streamUrl;
						throw HipeException(str.str());
					}
				}

				bool OK = _capture.grab();
				if (!OK)
				{
					return false;
				}
				_data.clear();
				cv::Mat frame;

				_capture.read(frame);

				while (frame.rows <= 0 && frame.cols <= 0)
				{

					if (!_capture.isOpened() || !_capture.grab())
					{
						return false;
					}
					_capture.read(frame);
				}


				_data.push_back(frame);

				return true;
			}

		};
	}
}
