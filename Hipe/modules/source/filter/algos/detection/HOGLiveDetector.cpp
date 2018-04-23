//@HIPE_LICENSE@
#include <filter/algos/detection/HOGLiveDetector.h>

HipeStatus filter::algos::HOGLiveDetector::process()
{
	{
		// Assert input data count is correct
		if (_connexData.size() != 2)
		{
			std::stringstream errorMessage;
			errorMessage << "Error in HOGLiveDetector: wrong input data count. Expected 2 (detectors and image), received " << _connexData.size() << "." << std::endl;
			throw HipeException(errorMessage.str());
		}

		// Pop and cast data
		data::Data data1 = _connexData.pop();
		data::Data data2 = _connexData.pop();

		data::ImageData* image;
		data::DlibDetectorData* detectors;

		if (data1.getType() == data::IODataType::IMGF)
		{
			image = static_cast<data::ImageData *>(&data1);
			detectors = static_cast<data::DlibDetectorData *>(&data2);
		}
		else
		{
			image = static_cast<data::ImageData *>(&data2);
			detectors = static_cast<data::DlibDetectorData *>(&data1);
		}

		// Assert data types are correct
		if (image->getType() != data::IODataType::IMGF || detectors->getType() != data::IODataType::DLIBDTCT)
			throw HipeException("Error in HogLiveDetector: wrong input data types");

		// Populate local fields to allow detect method in thread to run
		_currentInputFrame = image;
		auto detectorsVec = detectors->detectors_const();
		data::DlibDetectorData detectorsCopy(detectorsVec);
		pushInputDetector(detectorsCopy);

		data::ShapeData output = popOutputData();
		PUSH_DATA(output);
		_currentInputFrame = nullptr;
		return OK;
	}
}

void filter::algos::HOGLiveDetector::dispose()
{
	_isThreadRunning = false;

	_currentInputFrame = nullptr;

	if (_pFilterThread != nullptr)
	{
		_pFilterThread->join();
		delete _pFilterThread;
		_pFilterThread = nullptr;
	}
}

cv::Rect filter::algos::HOGLiveDetector::dlibToCVRect(const dlib::rectangle& r)
{
	return cv::Rect(cv::Point2i(r.left(), r.top()), cv::Point2i(r.right() + 1, r.bottom() + 1));
}

std::vector<cv::Rect> filter::algos::HOGLiveDetector::hogDetect(const cv::Mat& frame, const std::vector<dlib::object_detector<data::hog_trainer::image_scanner_type>>& detectors) const
{
	// Convert the OpenCV image to a dlib image.
	dlib::array2d<dlib::bgr_pixel> dlib_image;
	dlib::assign_image(dlib_image, dlib::cv_image<dlib::bgr_pixel>(frame));

	// Evaluate the detectors.
	std::vector<dlib::rectangle> dlib_rects = dlib::evaluate_detectors(detectors, dlib_image);

	std::vector<cv::Rect> rects;
	for (auto dlib_rect : dlib_rects)
	{
		rects.push_back(dlibToCVRect(dlib_rect));
	}

	return rects;
}

void filter::algos::HOGLiveDetector::startFilterThread()
{
	HOGLiveDetector* pThis = this;
	_pFilterThread = new boost::thread([pThis]
	{
		while (pThis->_isThreadRunning)
		{
			data::DlibDetectorData data;
			if (!pThis->_inputDataStack.trypop_until(data, 30))		// Try to pop during 30s
				continue;

			data::ShapeData output = pThis->detect(data.detectors_const());

			if (pThis->_outputDataStack.size() != 0)
				pThis->_outputDataStack.clear();

			pThis->_outputDataStack.push(output);
		}
	});
}

data::ShapeData filter::algos::HOGLiveDetector::detect(const std::vector<dlib::object_detector<data::hog_trainer::image_scanner_type> >& detectors)
{
	data::ShapeData output;
	if (_currentInputFrame == nullptr || detectors.empty())
		return output;

	if (_currentInputFrame->getMat().data)
		output << hogDetect(_currentInputFrame->getMat(), detectors);

	return output;
}

void filter::algos::HOGLiveDetector::pushInputDetector(const data::DlibDetectorData& detector)
{
	_inputDataStack.clear();
	_inputDataStack.push(detector);
}

data::ShapeData filter::algos::HOGLiveDetector::popOutputData()
{
	const int waitTime = 30;
	_outputDataStack.trypop_until(_currentOutputData, waitTime);

	return _currentOutputData;
}
