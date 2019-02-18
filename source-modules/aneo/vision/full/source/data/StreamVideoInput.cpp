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

#include <data/StreamVideoInput.h>

#pragma warning(push, 0) 
#include <opencv2/core/cvdef.h>
#pragma warning(pop) 

#include <coredata/IOData.h>

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
