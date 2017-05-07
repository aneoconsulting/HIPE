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
		class FileVideoInput : public IOData<VideoData, FileVideoInput>
		{
			boost::filesystem::path _filePath;
			cv::VideoCapture _capture;

			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }

		private:
			FileVideoInput() : IOData(IODataType::VIDF)
			{
				
			}

		public:

			FileVideoInput(const std::string & filePath) : IOData(IODataType::VIDF)
			{
				Data::registerInstance(new FileVideoInput());
				This()._filePath = filePath;
			}

			FileVideoInput(const FileVideoInput &data) : IOData(data._type)
			{
				Data::registerInstance(data._This);
				This()._filePath = This()._filePath;
			}

			cv::Mat newFrame()
			{
				if (!This()._capture.isOpened())
				{
					if (std::isdigit(This()._filePath.string().c_str()[0]))
					{
						This()._capture.open(atoi(This()._filePath.string().c_str()));
					} 
					else
						This()._capture.open(This()._filePath.string());

					if (!This()._capture.isOpened())
					{
						std::stringstream str;
						str << "Cannot open video : " << This()._filePath.string();
						throw HipeException(str.str());
					}
				}

				bool OK = This()._capture.grab();
				if (!OK)
				{
					return cv::Mat();
				}
				
				cv::Mat frame;
				
				This()._capture.read(frame);
				
				while (frame.rows <= 0 && frame.cols <= 0)
				{
					
					if (!This()._capture.isOpened() || !This()._capture.grab())
					{
						return cv::Mat();
					}
					This()._capture.read(frame);
				}
					

				

				return frame;
			}

		};
	}
}
