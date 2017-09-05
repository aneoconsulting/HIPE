#include <filter/Algos/FaceLandmark.h>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include "data/SquareCrop.h"


namespace filter
{
	namespace algos
	{
		using namespace std;

		void FaceLandmark::startDetectFace()
		{
			FaceLandmark* This = this;
			
			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::ImageData image;
					if (!This->imagesStack.trypop_until(image, 300))
						continue;

					cv::Mat matShapes = This->detectFaces(image);
					/*std::vector<cv::Rect> rects;

					for (dlib::rectangle & rect : This->dets)
					{
						cv::Rect cvRect;
						cvRect.x = rect.left() / 2;
						cvRect.y = rect.top() / 2;
						cvRect.height = (rect.bottom() - rect.top()) / 2;
						cvRect.width = (rect.right() - rect.left()) / 2;
						rects.push_back(cvRect);
					}*/
					/*data::SquareCrop crop(image, rects);*/

					if (This->shapes.size() != 0)
						This->shapes.clear();

				This->shapes.push(data::ImageData(matShapes));
				}
			});
		}

		void draw_polyline(cv::Mat &img, const dlib::full_object_detection& d, const int start, const int end, bool isClosed)
		{
			std::vector <cv::Point> points;
			for (int i = start; i <= end; ++i)
			{
				points.push_back(cv::Point(d.part(i).x(), d.part(i).y()));
			}
			cv::polylines(img, points, isClosed, cv::Scalar(255, 0, 0), 2, 16);

		}

		void render_face(cv::Mat &img, const dlib::full_object_detection& d)
		{
			DLIB_CASSERT
			(
				d.num_parts() == 68,
				"\n\t Invalid inputs were given to this function. "
				<< "\n\t d.num_parts():  " << d.num_parts()
			);

			draw_polyline(img, d, 0, 16);           // Jaw line
			draw_polyline(img, d, 17, 21);          // Left eyebrow
			draw_polyline(img, d, 22, 26);          // Right eyebrow
			draw_polyline(img, d, 27, 30);          // Nose bridge
			draw_polyline(img, d, 30, 35, true);    // Lower nose
			draw_polyline(img, d, 36, 41, true);    // Left eye
			draw_polyline(img, d, 42, 47, true);    // Right Eye
			draw_polyline(img, d, 48, 59, true);    // Outer lip
			draw_polyline(img, d, 60, 67, true);    // Inner lip

		}

		cv::Mat FaceLandmark::detectFaces(const data::ImageData & image)
		{
			try
			{
				dlib::array2d<unsigned char> img;
				dlib::cv_image<dlib::bgr_pixel> cimg(image.getMat());
				dlib::assign_image(img, cimg);

				std::vector<dlib::full_object_detection> local_shapes;
				cv::Mat matShapes = cv::Mat::zeros(image.getMat().size(), image.getMat().type());

				dlib::pyramid_up(img);

				// Now tell the face detector to give us a list of bounding boxes
				// around all the faces it can find in the image.
				dets = detector(img);

				for (unsigned long j = 0; j < dets.size(); ++j)
				{
					
					/*cout << "number of parts: " << shape.num_parts() << endl;
					cout << "pixel position of first part:  " << shape.part(0) << endl;
					cout << "pixel position of second part: " << shape.part(1) << endl;*/
					// You get the idea, you can get all the face part locations if
					// you want them.  Here we just store them in shapes so we can
					// put them on the screen.
					// Resize obtained rectangle for full resolution image. 
					dlib::rectangle r(
						(long)(dets[j].left() / 2),
						(long)(dets[j].top() / 2),
						(long)(dets[j].right() / 2),
						(long)(dets[j].bottom() / 2)
					);
					dlib::cv_image<dlib::bgr_pixel> cimg2(image.getMat());
					dlib::full_object_detection shape = pose_model(cimg2, r);
					local_shapes.push_back(shape);
					render_face(matShapes, shape);
				}
				return matShapes;
				
			}
			catch (exception& e)
			{
				cout << "\nexception thrown!" << endl;
				cout << e.what() << endl;
			}

			return cv::Mat::zeros(image.getMat().size(), image.getMat().type());
		}

		HipeStatus FaceLandmark::process()
		{
			cv::Mat im;
			cv::Mat im_small, im_display;

			if (!isStart.exchange(true))
			{
				dlib::deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
				startDetectFace();
			}

			std::vector<dlib::rectangle> faces;

			{
				data::ImageArrayData images = _connexData.pop();
				if (images.getType() == data::PATTERN)
				{
					throw HipeException("The resize object cant resize PatternData. Please Develop FaceLandmarkPatterData");
				}

				//FaceLandmark all images coming from the same parent
				for (auto &myImage : images.Array())
				{
					if (count_frame % skip_frame == 0)
					{
						if (imagesStack.size() != 0)
							imagesStack.clear();
						imagesStack.push(myImage);
					}
					count_frame++;
				}
				//TODO manage list of SquareCrop. For now consider there only one SquareCrop
				data::ImageData popShape;
				
				if (shapes.trypop_until(popShape, 30)) // wait 30ms no more
				{
					_connexData.push(popShape);
					tosend = popShape;
				}
				else if (tosend.empty())
				{
					_connexData.push(images.Array()[0]);
				}
				else {

					_connexData.push(tosend);
				}
			}
			return OK;
		}
	}
}
