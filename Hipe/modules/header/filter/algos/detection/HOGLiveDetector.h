//@HIPE_LICENSE@
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
