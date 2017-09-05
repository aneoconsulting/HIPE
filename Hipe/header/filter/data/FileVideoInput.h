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
		/**
		 * \brief FileVideoInput is the data type used to handle a video. Uses OpenCV.
		 */
		class FileVideoInput : public VideoData<FileVideoInput>
		{
			/**
			 * \brief Path of the video
			 */
			boost::filesystem::path _filePath;
			/**
			 * \brief Handles the reading of the video's frames
			 */
			cv::VideoCapture _capture;
			/**
			 * \brief If set to true, loops the video at the end of the playback
			 */
			bool _loop;

			cv::Mat asOutput() { return cv::Mat::zeros(0, 0, CV_8UC1); }

		private:
			FileVideoInput() : VideoData(IODataType::VIDF)
			{
				
			}

		public:

			/**
			 * \brief 
			 * \param filePath the path to the video file
			 * \param loop Should the video loop on playback end
			 */
			FileVideoInput(const std::string & filePath, bool loop) : VideoData(IODataType::VIDF)
			{
				Data::registerInstance(new FileVideoInput());
				This()._filePath = filePath;
				This()._loop = loop;
			}

			FileVideoInput(const FileVideoInput &data) : VideoData(data._type)
			{
				Data::registerInstance(data._This);
				This()._filePath = data.This_const()._filePath;
				This()._loop = data.This_const()._loop;
			}

			/**
			 * \brief Opens a video file from its path, if valid.
			 */
			void openFile()
			{
				if (This()._capture.isOpened() && !This()._capture.grab())
				{
					This()._capture.open(This()._filePath.string());
					This()._capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
					This()._capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
				}

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
			}

			/**
			 * \brief Get the next frame of the video
			 * \return Return the next frame of the video as a \see Data object, or a black one if the plaback already ended and the loop option is disabled
			 */
			Data newFrame()
			{
				openFile();

				bool OK = This()._capture.grab();
				if (!OK)
				{
					if (This()._loop)
					{
						openFile();
					}
					else
					{
						return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));
					}
				}
				

				cv::Mat frame;
				
				This()._capture.read(frame);
				int retry = 150;
				while (frame.rows <= 0 && frame.cols <= 0 && retry >= 0)
				{
					if (This()._loop && !This()._capture.grab())
					{
						
						openFile();
					}

					if (!(This()._loop) && (!This()._capture.isOpened() || !This()._capture.grab()))
					{
						return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));
					}

					This()._capture.read(frame);
					if (This()._loop)
						retry--;
				}
			
				return static_cast<Data>(ImageData(frame));;
			}


			/**
			 * [TODO]
			 * \brief Checks if the FileVideo is empty.
			 * \return  Returns true if the object doesn't contain any data or the video is not opened, false either.
			 */
			bool empty() const
			{
				return ! (This_const()._capture.isOpened());
			}

		};
	}
}
