#include <filter/algos/detection/FaceDetection.h>
#include <dlib/image_processing/frontal_face_detector.h>

#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		using namespace std;

		void FaceDetection::startDetectFace()
		{
			FaceDetection* This = this;
			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::ImageData image;
					if (!This->imagesStack.trypop_until(image, 300))
						continue;

					This->detectFaces(image);
					std::vector<cv::Rect> rects;

					for (dlib::rectangle & rect : This->dets)
					{
						cv::Rect cvRect;
						cvRect.x = std::max<int>(rect.left() / 2 - 20, 0);
						cvRect.y = std::max<int>(rect.top() / 2 - 20, 0);
						cvRect.height = std::min<int>((rect.bottom() - rect.top()) / 2 + 40, image.getMat().size().width);
						cvRect.width = std::min<int>((rect.right() - rect.left()) / 2 + 40, image.getMat().size().width);
						rects.push_back(cvRect);
					}
					data::ShapeData crop;
					crop << rects;

					if (This->crops.size() != 0)
						This->crops.clear();

					This->crops.push(crop);
				}
			});
		}

		void FaceDetection::detectFaces(const data::ImageData & image)
		{
			try
			{
				dlib::array2d<unsigned char> img;
				dlib::cv_image<dlib::bgr_pixel> cimg(image.getMat());
				dlib::assign_image(img, cimg);


				// Make the image bigger by a factor of two.  This is useful since
				// the face detector looks for faces that are about 80 by 80 pixels
				// or larger.  Therefore, if you want to find faces that are smaller
				// than that then you need to upsample the image as we do here by
				// calling pyramid_up().  So this will allow it to detect faces that
				// are at least 40 by 40 pixels in size.  We could call pyramid_up()
				// again to find even smaller faces, but note that every time we
				// upsample the image we make the detector run slower since it must
				// process a larger image.
				dlib::pyramid_up(img);


				// Now tell the face detector to give us a list of bounding boxes
				// around all the faces it can find in the image.
				dets = detector(img);

				//std::cout << "Number of faces detected: " << dets.size() << std::endl;
				//// Now we show the image on the screen and the face detections as
				//// red overlay boxes.
				//win.clear_overlay();
				//win.set_image(img);
				//win.add_overlay(dets, dlib::rgb_pixel(255,0,0));


				//std::cout << "Hit enter to process the next image..." << std::endl;
				//std::cin.get();
			}
			catch (exception& e)
			{
				cout << "\nexception thrown!" << endl;
				cout << e.what() << endl;
			}
		}

		HipeStatus FaceDetection::process()
		{
			cv::Mat im;
			cv::Mat im_small, im_display;
			std::vector<dlib::rectangle> faces;

			if (!isInit.exchange(true))
			{
				detector = dlib::get_frontal_face_detector();

				dlib::deserialize(file_predictor_dat) >> pose_model;
				isStart = true;
				startDetectFace();
			}

			{
				data::ImageArrayData images = _connexData.pop();
				if (images.getType() == data::PATTERN)
				{
					throw HipeException("The resize object cant resize PatternData. Please Develop FaceDetectionPatterData");
				}

				//FaceDetection all images coming from the same parent
				for (auto &myImage : images.Array())
				{
					if (skip_frame == 0 || count_frame % skip_frame == 0)
					{
						if (imagesStack.size() != 0)
							imagesStack.clear();
						imagesStack.push(myImage);
					}
					count_frame++;
				}
				//TODO manage list of SquareCrop. For now consider there only one SquareCrop
				data::ShapeData popShapeData;
				if (crops.trypop_until(popShapeData, 30)) // wait 30ms no more
				{
					PUSH_DATA(popShapeData);
					tosend = popShapeData;
				}
				else {

					PUSH_DATA(tosend);
				}
			}
			return OK;
		}
	}
}