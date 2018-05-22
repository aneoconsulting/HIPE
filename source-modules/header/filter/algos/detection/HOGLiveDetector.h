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
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <coredata/IOData.h>
#include <data/ImageData.h>
#include <data/ShapeData.h>
#include <data/DlibDetectorData.h>


namespace filter
{
	namespace algos
	{
		class HOGLiveDetector : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ShapeData);
			REGISTER(HOGLiveDetector, ()), _connexData(data::INDATA)
			{
				_isThreadRunning = true;
				_pFilterThread = nullptr;
				_currentInputFrame = nullptr;

				skip_frames = 4;

				startFilterThread();
			}

			REGISTER_P(int, skip_frames);

			std::atomic<bool> _isThreadRunning;										//<! boolean to know if the main thread of the filter is already running
			boost::thread* _pFilterThread;											//<! pointer to the filter's asynchronous thread

			core::queue::ConcurrentQueue<data::DlibDetectorData> _inputDataStack;	//<! [TODO] The queue containing the frames to process.
			core::queue::ConcurrentQueue<data::ShapeData> _outputDataStack;			//<! [TODO] The queue containing the results of all the detections to process

			data::ImageData* _currentInputFrame;
			data::ShapeData _currentOutputData;

		public:
			HipeStatus process() override;
			void dispose() override;

		private:
			static inline cv::Rect dlibToCVRect(const dlib::rectangle& r);
			std::vector<cv::Rect> hogDetect(const cv::Mat& frame, const std::vector<dlib::object_detector<data::hog_trainer::image_scanner_type> >& detectors) const;


		private:
			void startFilterThread();

			data::ShapeData detect(const std::vector<dlib::object_detector<data::hog_trainer::image_scanner_type> >& detectors);
			void pushInputDetector(const data::DlibDetectorData& detector);
			data::ShapeData popOutputData();
		};

		ADD_CLASS(HOGLiveDetector, skip_frames);
	}
}
