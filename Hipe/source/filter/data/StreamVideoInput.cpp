#include <filter/data/StreamVideoInput.h>

namespace filter
{
	namespace data
	{
		cv::Mat StreamVideoInput::asOutput() const
		{
			return cv::Mat::zeros(0, 0, CV_8UC1);
		}

		StreamVideoInput::StreamVideoInput(const std::string & url) : VideoData(filter::data::IODataType::STRMVID)
		{
			_filePath = url;

			std::string instanceName = "RTSPCapture";
			_capture.reset(CaptureVideoFactory::getCaptureVideo(instanceName, url), [](CaptureVideo*) {});
			_capture.get()->create();
			_capture.get()->open();
		}

		StreamVideoInput::StreamVideoInput(StreamVideoInput &data) : VideoData(data)
		{
			_filePath = data._filePath;

			std::string instanceName = "RTSPCapture";

			_capture.reset(CaptureVideoFactory::getCaptureVideo(instanceName, _filePath.string()), [](CaptureVideo*) {});
			
			if (_capture.get()->create() != OK)
				throw HipeException("Cannot create Streaming capture");

			if (_capture.get()->open() != OK)
				throw HipeException("Cannot open streaming capture");
		}
	}
}