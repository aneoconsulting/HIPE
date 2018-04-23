//@HIPE_LICENSE@
#pragma once

#include <data/ImageData.h>
#include <coredata/IODataType.h>
#include <string>

#include <data/VideoData.h>

#include <coredata/data_export.h>

#pragma warning(push, 0) 
#include <boost/filesystem/path.hpp>
#include <opencv2/opencv.hpp>
#pragma warning(pop) 


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
			_filePath = filePath;
			This()._filePath = filePath;
			This()._loop = loop;
			_loop = loop;
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
		 * \brief Close Source video file
		 */
		void closeFile();

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

	public:
		boost::filesystem::path getFilePath() const
		{
			return _filePath;
		}

		void setFilePath(const boost::filesystem::path& filePath)
		{
			_filePath = filePath;
			This()._filePath = filePath;
		}

	
	};
}
