//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

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
