#pragma once

#pragma once
#include <iostream>

#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>


#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>

#include <dlib/opencv.h>

#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <filter/data/SquareCrop.h>
#include <filter/filter_export.h>

namespace filter {
	namespace data {
		class ImageData;
	}
}

namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT FaceLandmark : public filter::IFilter
		{
			int count_frame;
			dlib::frontal_face_detector detector;
			std::vector<dlib::rectangle> dets;
			dlib::shape_predictor pose_model;

			std::atomic<bool> isStart;
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;
			core::queue::ConcurrentQueue<data::ImageData> shapes;
			data::ImageData tosend;

			boost::thread *thr_server;
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageData);

			REGISTER(FaceLandmark, ()), _connexData(data::INDATA)
			{
				count_frame = 0;
				skip_frame = 4;
				detector = dlib::get_frontal_face_detector();
				isStart = false;
				thr_server = nullptr;



				

				
			}

			REGISTER_P(int, skip_frame);


			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			void startDetectFace();

			cv::Mat detectFaces(const data::ImageData & image);


			HipeStatus process();


			virtual void dispose()
			{
				isStart = false;

				if (thr_server != nullptr) {
					thr_server->join();
					delete thr_server;
					thr_server = nullptr;
				}
			}
		};

		ADD_CLASS(FaceLandmark, skip_frame);
	}
}
