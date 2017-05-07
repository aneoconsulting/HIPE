#include <filter/data/StreamVideoInput.h>

namespace filter
{
	namespace data
	{
		cv::Mat StreamVideoInput::asOutput() const
		{
			return cv::Mat::zeros(0, 0, CV_8UC1);
		}

		StreamVideoInput::StreamVideoInput(): IOData(STRMVID)
		{
		}

		StreamVideoInput::StreamVideoInput(const std::string & url) : IOData(STRMVID)
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

		cv::Mat StreamVideoInput::newFrame() const
		{
			cv::Mat data;
			HipeStatus hipe_status = This()._capture.get()->read(data);
			if (hipe_status == UNKOWN_ERROR)
				throw HipeException("Error grabbing frame");
			if (hipe_status == END_OF_STREAM)
				data = cv::Mat::zeros(0, 0, CV_8U);

			return data;
		}

		StreamVideoInput::StreamVideoInput(const StreamVideoInput &data) : IOData(data._type)
		{
			Data::registerInstance(data._This);
			This()._filePath = data._filePath;

			std::string instanceName = "RTSPCapture";

			This()._capture.reset(CaptureVideoFactory::getInstance()->getCaptureVideo(instanceName, _filePath.string()), [](CaptureVideo*) {});
			
			if (This()._capture.get()->create() != OK)
				throw HipeException("Cannot create Streaming capture");

			if (This()._capture.get()->open() != OK)
				throw HipeException("Cannot open streaming capture");
		}
	}
}