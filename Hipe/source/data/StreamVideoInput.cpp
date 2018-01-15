#include <data/StreamVideoInput.h>
#include <opencv2/core/cvdef.h>
#include <data/IOData.h>
#include "core/HipeStatus.h"

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
#ifdef OLD_CODE
		std::string instanceName = "RTSPCapture";
		This()._capture.reset(CaptureVideoFactory::getInstance()->getCaptureVideo(instanceName, url), [](CaptureVideo*) {});
		This()._capture.get()->create();
		This()._capture.get()->open();
#endif
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
#ifdef OLD_CODE
		HipeStatus hipe_status = This_const()._capture.get()->read(data);
		if (hipe_status == UNKOWN_ERROR)
			throw HipeException("Error grabbing frame");
		if (hipe_status == END_OF_STREAM)
			return static_cast<Data>(ImageData(cv::Mat::zeros(0, 0, 0)));
#endif
		return static_cast<Data>(ImageData(data));
	}

	bool StreamVideoInput::empty() const
	{
		cv::Mat data;
#ifdef OLD_CODE
		return This_const()._capture.get()->read(data) != OK;;
#else
		return false;
#endif
	}

	StreamVideoInput::StreamVideoInput(const StreamVideoInput &data) : VideoData(data._type)
	{
		Data::registerInstance(data._This);
		This()._filePath = data.This_const()._filePath;
#ifdef OLD_CODE
		std::string instanceName = "RTSPCapture";

		//This()._capture.reset(CaptureVideoFactory::getInstance()->getCaptureVideo(instanceName, _filePath.string()), [](CaptureVideo*) {});
		This()._capture = data.This_const()._capture;

		if (This()._capture.get()->create() != OK)
			throw HipeException("Cannot create Streaming capture");

		if (This()._capture.get()->open() != OK)
			throw HipeException("Cannot open streaming capture");
#endif
	}
}
