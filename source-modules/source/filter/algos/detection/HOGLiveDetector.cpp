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
#if defined(USE_DLIB) &&  defined(__ALTIVEC__)
	//issue order of header for vector keyword call it before 
	#include <dlib/simd.h>
#endif
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
