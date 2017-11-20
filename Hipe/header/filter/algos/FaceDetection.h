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
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
#include <data/ShapeData.h>
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
		/**
		 *  \var FaceDetection::skip_frame
		 *  The number of frames to skip between each detection.
		 */

		/**
		 * \todo
		 * \brief The FaceDetection filter is used to detect faces in an image or a video.
		 * 
		 *  The filter uses dlib.
		 *  It will output a SquareCrop object containing all the found faces.
		 */
		class FILTER_EXPORT FaceDetection : public filter::IFilter
		{
			int count_frame;	//<! The number of already processed frames 
			dlib::frontal_face_detector detector;	//<! The detector object used to find the faces in the images 
			std::vector<dlib::rectangle> dets;		//<! The list of all the found faces
			dlib::shape_predictor pose_model;		//<! The model (machine learning) used to identify the faces

			std::atomic<bool> isStart;				//<! [TODO] Boolean used to know when the thread is running and the faces should be detected ?
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;	//<! [TODO] The queue containing the frames to process
			core::queue::ConcurrentQueue<data::ShapeData> crops;		//<! [TODO] The queue containing the cropped faces from the images
			data::ShapeData tosend;	//<! The found faces to output to the ConnexData port

			boost::thread *thr_server;	//<! [TODO] Pointer to the face detection task
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ShapeData);

			REGISTER(FaceDetection, ()), _connexData(data::INDATA)
			{
				count_frame = 0;
				skip_frame = 4;
				detector = dlib::get_frontal_face_detector();
				isStart = true;
				thr_server = nullptr;



				dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

				startDetectFace();
			}

			REGISTER_P(int, skip_frame);


			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			/**
			 * \brief Detects the faces in images. Runs as a separate thread.
			 *  Fetch its images from the imagesStack queue then feed the detectFaces method.
			 */
			void startDetectFace();

			/**
			 * \brief Find faces, if present, on an image.
			 * \param image The image to process and on which we'll try to find faces.
			 */
			void detectFaces(const data::ImageData & image);


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

		ADD_CLASS(FaceDetection, skip_frame);
	}
}
