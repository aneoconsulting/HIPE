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

#include <data/FileVideoInput.h>
#include <core/HipeTimer.h>

namespace data
{
	inline cv::Mat FileVideoInput::asOutput()
	{
		return cv::Mat::zeros(0, 0, CV_8UC1);
	}

	void FileVideoInput::openFile()
	{
		if (This()._capture.isOpened() && !This()._capture.grab())
		{
			This()._capture.release();
			
			This()._capture.open(This()._filePath.string());

			if (std::isdigit(This()._filePath.string().c_str()[0]))
			{
				This()._capture.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
				This()._capture.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
			}
			
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
			{
				This()._capture.open(This()._filePath.string());
				
			}

			if (!This()._capture.isOpened())
			{
				std::stringstream str;
				str << "Cannot open video : " << This()._filePath.string();
				throw HipeException(str.str());
			}
		}
	}

	void FileVideoInput::closeFile()
	{
		if (This()._capture.isOpened())
		{
			This()._capture.release();
		}
	}

	Data FileVideoInput::newFrame()
	{
		static core::HipeTimer perFs;

		std::cout << "FPS : " << 1.0 / ( perFs.stop().getElapseTimeInMili() / 1000.0) << " ms" << std::endl;

		if (This()._capture.isOpened() && !This()._capture.grab())
			return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));
		
		openFile();
		 double fps = This()._capture.get(cv::CAP_PROP_FPS);
		//std::cout << "Frames per second using video.get(CV_CAP_PROP_FPS) : " << fps << std::endl;
		
		bool OK = This()._capture.grab();
		if (!OK)
		{
			if (This()._loop)
			{
				closeFile();
				openFile();
			}
			else
			{
				
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
		perFs.start();
		return static_cast<Data>(ImageData(frame));;
	}

	bool FileVideoInput::empty() const
	{
		return !(This_const()._capture.isOpened());
	}
}
