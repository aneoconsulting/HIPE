//@HIPE_LICENSE@
#include <data/FileVideoInput.h>

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
				This()._capture.open(This()._filePath.string());

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
		
		if (This()._capture.isOpened() && !This()._capture.grab())
			return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));
		
		openFile();

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

		return static_cast<Data>(ImageData(frame));;
	}

	bool FileVideoInput::empty() const
	{
		return !(This_const()._capture.isOpened());
	}
}
