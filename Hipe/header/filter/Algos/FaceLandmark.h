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
		/**
		 * \brief 
		 */
		class FILTER_EXPORT FaceLandmark : public filter::IFilter
		{
			int count_frame;							//<! The number of frames already processed
			dlib::frontal_face_detector detector;		//<! The detector object used to find the faces in the images
			std::vector<dlib::rectangle> dets;			//<! The list of all the found faces
			dlib::shape_predictor pose_model;			//<! The model(machine learning) used to identify the shape of the faces

			std::atomic<bool> isStart;										//<! [TODO] Boolean used to know when the thread is running and the faces should be detected ?
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;		//<! [TODO] The queue containing the frames to process
			core::queue::ConcurrentQueue<data::ImageData> shapes;			//<! [TODO] The shapes of the found faces
			data::ImageData tosend;			//<! The image containing the drawn facial landmarks to output to the ConnexData port

			boost::thread *thr_server;		//<! [TODO] Pointer to the face detection task
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

			REGISTER_P(int, skip_frame);	//<! The number of frames to skip between each detection


			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			/**
			 * \brief Detects the faces in images. Runs as a separate thread.
			 *  Fetch its images from the imagesStack queue then feed the detectFaces method.
			 */
			void startDetectFace();

			/**
			 * \brief Find faces, if present, then their landmarks.
			 * \param image The image to process to try finding faces
			 * \return Returns the processed frame with the landmarks of the found faces drawn on it
			 */
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

		/**
		 * \brief Draws on an image a complex line made of multiple points
		 * \param img The image on which we want to draw the lines
		 * \param d [TODO] The detected facial landmarks
		 * \param start [TODO] The index of the first point of the line in the container
		 * \param end [TODO] The index of the last point of the line in the container
		 * \param isClosed [TODO] Should it be a line or a polygon (draw a line from the last point to the first)
		 */
		void draw_polyline(cv::Mat &img, const dlib::full_object_detection& d, const int start, const int end, bool isClosed = false);
		/**
		 * \brief Draws facial landmarks on an image, using the draw_polyline method.
		 * \param img The image on which the face was found
		 * \param d [TODO] The detected facial landmarks
		 */
		void render_face(cv::Mat &img, const dlib::full_object_detection& d);

		ADD_CLASS(FaceLandmark, skip_frame);
	}
}
