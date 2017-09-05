#include <filter/data/StreamVideoInput.h>

namespace filter
{
	namespace data
	{
		cv::Mat StreamVideoInput::asOutput() const
		{
			return cv::Mat::zeros(0, 0, CV_8UC1);
		}

		StreamVideoInput::StreamVideoInput() : VideoData(STRMVID)
		{
		}

		StreamVideoInput::StreamVideoInput(const std::string & url) : VideoData(STRMVID)
		{
			Data::registerInstance(new StreamVideoInput());

			This()._filePath = url;

			std::string instanceName = "RTSPCapture";
			This()._capture.reset(CaptureVideoFactory::getInstance()->getCaptureVideo(instanceName, url), [](CaptureVideo*) {});
			This()._capture.get()->create();
			This()._capture.get()->open();
		}


		StreamVideoInput::~StreamVideoInput()
		{
		}

		void copyTo(StreamVideoInput& left)
		{
			throw HipeException("Not yet implemented copy of StreamVideoInput");
		}

		Data StreamVideoInput::newFrame()
		{
			cv::Mat data;
			HipeStatus hipe_status = This_const()._capture.get()->read(data);
			if (hipe_status == UNKOWN_ERROR)
				throw HipeException("Error grabbing frame");
			if (hipe_status == END_OF_STREAM)
				return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));

			return static_cast<Data>(ImageData(data));
		}

		StreamVideoInput::StreamVideoInput(const StreamVideoInput &data) : VideoData(data._type)
		{
			Data::registerInstance(data._This);
			This()._filePath = data.This_const()._filePath;

			std::string instanceName = "RTSPCapture";

			//This()._capture.reset(CaptureVideoFactory::getInstance()->getCaptureVideo(instanceName, _filePath.string()), [](CaptureVideo*) {});
			This()._capture = data.This_const()._capture;

			if (This()._capture.get()->create() != OK)
				throw HipeException("Cannot create Streaming capture");

			if (This()._capture.get()->open() != OK)
				throw HipeException("Cannot open streaming capture");
		}

	}
}