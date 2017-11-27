#pragma once
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>

#include <data/IOData.h>
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
			}

			REGISTER_P(char, unused);

		public:
			HipeStatus process() override
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

				// Detect objects in image
				std::vector<cv::Rect> detectedObjects = hogDetect(image->getMat(), detectors->detectors_const(), detectors->mutex_ptr());

				// Output ShapeData
				data::ShapeData output;
				output << detectedObjects;

				_connexData.push(output);
				return OK;
			}

		private:
			static inline cv::Rect dlibToCVRect(const dlib::rectangle& r)
			{
				return cv::Rect(cv::Point2i(r.left(), r.top()), cv::Point2i(r.right() + 1, r.bottom() + 1));
			};

			std::vector<cv::Rect> hogDetect(const cv::Mat& frame, const std::vector<dlib::object_detector<::data::hog_trainer::image_scanner_type> >& detectors, std::shared_ptr<boost::shared_mutex> detectors_mutex) const
			{
				// Convert the OpenCV image to a dlib image.
				dlib::array2d<dlib::bgr_pixel> dlib_image;
				dlib::assign_image(dlib_image, dlib::cv_image<dlib::bgr_pixel>(frame));

				// Evaluate the detectors.
				boost::upgrade_lock<boost::shared_mutex> lock(*detectors_mutex);
				std::vector<dlib::rectangle> dlib_rects = dlib::evaluate_detectors(detectors, dlib_image);
				lock.unlock();

				std::vector<cv::Rect> rects;
				for (auto dlib_rect : dlib_rects)
				{
					rects.push_back(dlibToCVRect(dlib_rect));
				}

				return rects;
			}
		};

		ADD_CLASS(HOGLiveDetector, unused);
	}
}
