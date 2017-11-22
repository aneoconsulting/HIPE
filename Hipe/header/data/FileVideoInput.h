#pragma once

#include <data/ImageData.h>
#include <data/IODataType.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include "VideoData.h"

#include <data/data_export.h>

namespace data
{
	/**
	 * \brief FileVideoInput is the data type used to handle a video. Uses OpenCV.
	 */
	class DATA_EXPORT FileVideoInput : public VideoData<FileVideoInput>
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

		cv::Mat asOutput();

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
		void openFile();

		/**
		 * \brief Get the next frame of the video
		 * \return Return the next frame of the video as a \see Data object, or a black one if the plaback already ended and the loop option is disabled
		 */
		Data newFrame();

		/**
		 * \todo
		 * \brief Checks if the FileVideo is empty.
		 * \return  Returns true if the object doesn't contain any data or the video is not opened, false either.
		 */
		bool empty() const;
	};
}
