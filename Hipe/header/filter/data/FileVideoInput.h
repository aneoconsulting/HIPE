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
		class FileVideoInput : public VideoData<FileVideoInput>
		{
			boost::filesystem::path _filePath;
			cv::VideoCapture _capture;

			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }

		private:
			FileVideoInput() : VideoData(IODataType::VIDF)
			{
				
			}

		public:

			FileVideoInput(const std::string & filePath) : VideoData(IODataType::VIDF)
			{
				Data::registerInstance(new FileVideoInput());
				This()._filePath = filePath;
			}

			FileVideoInput(const FileVideoInput &data) : VideoData(data._type)
			{
				Data::registerInstance(data._This);
				This()._filePath = This()._filePath;
			}

			Data newFrame()
			{
				if (!This()._capture.isOpened())
				{

					if (std::isdigit(This()._filePath.string().c_str()[0]))
					{
						This()._capture.open(atoi(This()._filePath.string().c_str()));
						This()._capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
						This()._capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
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
					return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));
				}
				

				cv::Mat frame;
				
				This()._capture.read(frame);
				int retry = 150;
				while (frame.rows <= 0 && frame.cols <= 0 && retry >= 0)
				{
					if (!This()._capture.isOpened() || !This()._capture.grab())
					{
						return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));
					}
					This()._capture.read(frame);
					retry--;
				}
			
				return static_cast<Data>(ImageData(frame));;
			}

			bool empty() const
			{
				return ! (This_const()._capture.isOpened());
			}

		};
	}
}
