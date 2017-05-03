#pragma once


#pragma once

#include <filter/data/ImageData.h>
#include <filter/data/IODataType.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include "VideoData.h"

namespace filter
{
	namespace data
	{
		class FileVideoInput : public VideoData
		{
			boost::filesystem::path _filePath;
			cv::VideoCapture _capture;

			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }

		public:

			FileVideoInput(const std::string & filePath) : VideoData(IODataType::VIDF)
			{
				_filePath = filePath;
			}

			FileVideoInput(FileVideoInput &data) : VideoData(data)
			{
				_filePath = data._filePath;
			}

			bool newFrame()
			{
				if (!_capture.isOpened())
				{
					if (std::isdigit(_filePath.string().c_str()[0]))
					{
						_capture.open(atoi(_filePath.string().c_str()));
					} 
					else
						_capture.open(_filePath.string());

					if (!_capture.isOpened())
					{
						std::stringstream str;
						str << "Cannot open video : " << _filePath.string();
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
