#pragma once
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

 #define WITH_DLIB 1
// #undef WITH_DLIB

#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

#ifdef WITH_DLIB
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>


typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6> > image_scanner_type;
#endif //WITH_DLIB

namespace fs = boost::filesystem;

/*!
	@brief Compare contours by area for sorting.
*/
bool
compare_contour_area(std::vector<cv::Point> const & a, std::vector<cv::Point> const & b)
{
	return cv::contourArea(a) > cv::contourArea(b);
}


template <typename P>
void
contour_center(std::vector<cv::Point> const & cntr, P & point)
{
	cv::Moments cntr_moments;
	cntr_moments = cv::moments(cntr);
	point.x = cntr_moments.m10/cntr_moments.m00;
	point.y = cntr_moments.m01/cntr_moments.m00;
}



#ifdef WITH_DLIB
void
fhog_to_mat(dlib::object_detector<image_scanner_type> const & detector, cv::Mat & image)
{
	dlib::matrix<unsigned char> fhog = dlib::draw_fhog(detector);
	cv::Mat tmp(fhog.nr(), fhog.nc(), CV_8UC1);

	int i = 0;
	int j = 0;
	int cols = fhog.nc();
	unsigned char * row = tmp.ptr<unsigned char>(i);
	for (auto pixel: fhog)
	{
		row[j++] = pixel;
		if (j >= cols)
		{
			j = j % cols;
			row = tmp.ptr<unsigned char>(++i);
		}
	}
	image = tmp;
}



static
dlib::rectangle
cv_to_dlib_rect(cv::Rect r)
{
  return dlib::rectangle((long)r.tl().x, (long)r.tl().y, (long)r.br().x - 1, (long)r.br().y - 1);
}

static
cv::Rect
dlib_to_cv_rect(dlib::rectangle r)
{
  return cv::Rect(cv::Point2i(r.left(), r.top()), cv::Point2i(r.right() + 1, r.bottom() + 1));
}
#endif //WITH_DLIB


/*!
  @brief Save a sequentially numbered image.
  @param name The image name.
  @param img The OpenCV image.
*/
void
sequential_image_save(std::string name, cv::Mat img, cv::Rect rect)
{
	//! @brief Index for sequential naming.
	static unsigned int index = 1;
	//! @brief The image output directory.
	fs::path dir {"tmp"};
	//! @brief The image filename.
	fs::path img_filename, xml_filename;
	//! @brief Temporary stringstream for building the indexed filename.
	std::stringstream indexed_name;
	//! @brief The final output path.`
	std::string path;
	//! @brief Compression parameters for the image.
	std::vector<int> compression_params {CV_IMWRITE_JPEG_QUALITY, 95};

	fs::create_directory(dir);
	indexed_name << std::setfill('0') << std::setw(4) << index;
	indexed_name << "_" << name;
	img_filename = indexed_name.str() + ".jpg";
	path = (dir / img_filename).string();
	std::cout << "Saving image to " << path << " [" << img.rows << "x" << img.cols << "]" << std::endl;
	cv::imwrite(path, img);

	xml_filename = indexed_name.str() + ".xml";
	path = (dir / xml_filename).string();
	std::cout << "Saving xml to " << path << std::endl;
	std::ofstream xmlfile;
	xmlfile.open(path);
	// The path object automatically inserts quotes.
	xmlfile << "  <image file=" << img_filename << ">\n";
	xmlfile << "    <box top=\""<< rect.y << "\" left=\"" << rect.x << "\" ";
	xmlfile << "width=\"" << rect.width << "\" height=\"" << rect.height << "\"/>\n";
	xmlfile << "  </image>\n";
	xmlfile.close();

  index += 1;
}



class HogTrainer
{
	private:
		static unsigned char const DEFAULT_HUE = 164; // pink
		static char const DELTA_H = 7, DELTA_S = 50, DELTA_V = 75;

		cv::Scalar hsv_min {90-DELTA_H, 50, 40};
		cv::Scalar hsv_max {90+DELTA_H, 255, 255};
		cv::Scalar center_pixel {0,0,0};
		cv::Scalar calibration_pixel {0,0,0};

		int x_center=0, y_center=0;

		//! @brief Primary camera number.
		int cam_prime = 0;
		cv::VideoCapture vc_prime;

		unsigned char hue = DEFAULT_HUE;
		unsigned char center_h=0, center_s=0, center_v=0;

		double conf_interval = 2;
		clock_t action_begin {0};

		bool running = false;
		bool calibrated = false;
		bool save = false;
		bool set_min_cntr_area = false;
		double min_cntr_area = 0;

		std::vector<std::vector<cv::Point>> cntrs;


		std::vector<cv::Rect> viewports {cv::Rect {0,0,150,150}};
		int vp_offset_x = 0;
		int vp_offset_y = 175;

		bool vp_conf = false;

		bool pos_conf = false;

		std::map<const char *, cv::Mat> windows;


#ifdef WITH_DLIB
		bool detect = false;
		bool train = false;

		dlib::array<dlib::array2d<unsigned char>> images_train;
		std::vector<std::vector<dlib::rectangle>> boxes_train;

		std::vector<dlib::object_detector<image_scanner_type>> detectors;
		std::vector<bool> active;
#else //WITH_DLIB
    // Just for testing the toggle function.
		std::vector<bool> active {true, false, false, true, false};
#endif //WITH_DLIB
		bool toggle_active = false;

	public:
		HogTrainer(
			int cp=0,
      unsigned char h=DEFAULT_HUE
		):
			cam_prime{cp},
			hue{h}
		{
			vc_prime = cv::VideoCapture(cam_prime);
		};

		~HogTrainer()
		{
			vc_prime.release();
			cv::destroyAllWindows();
		}


		void
		set_hsv_ranges()
		{
			hsv_min[1] = (center_s > DELTA_S ? center_s - DELTA_S : 0);
			hsv_max[1] = (255 - DELTA_S > center_s ? center_s + DELTA_S : 255);
			hsv_min[2] = (center_v > DELTA_V ? center_v - DELTA_V : 0);
			hsv_max[2] = (255 - DELTA_V > center_v ? center_v + DELTA_V : 255);
		}



		void
		calibrate()
		{
			calibration_pixel = center_pixel;
			hue = center_h;
			set_hsv_ranges();
			calibrated = true;
		}



		void
		get_hsv_mask(cv::Mat const & frame, cv::Mat & mask)
		{
			cv::Mat hsv;
			int hue_shift = 90 - hue;

			cv::GaussianBlur(frame, hsv, {5,5}, 0);
			cv::cvtColor(hsv, hsv, cv::COLOR_BGR2HSV);
			// Use center pixel.
			center_pixel = hsv.at<cv::Vec3b>(y_center, x_center);
			center_h = center_pixel[0];
			center_s = center_pixel[1];
			center_v = center_pixel[2];
			/*
				The center pixel BGR color is used to diplay info about detection
				contour size thresholds.
			*/
			center_pixel = frame.at<cv::Vec3b>(y_center, x_center);

			if (hue_shift)
			{
				for (int i=0; i<hsv.rows; ++i)
				{
					cv::Vec3b * row = hsv.ptr<cv::Vec3b>(i);
					for (int j=0; j<hsv.cols; ++j)
					{
						row[j][0] = (row[j][0] + hue_shift) % 180;
					}
				}
			}

			// mask is a binary mask
			cv::inRange(hsv, hsv_min, hsv_max, mask);
			cv::GaussianBlur(mask, mask, {5, 5}, 0);
		}



		void
		draw()
		{
			for (auto const & window : windows)
			{
				cv::imshow(window.first, window.second);
			}
		}

		void
		move_windows()
		{
			int x = 0;
			for (auto const & window : windows)
			{
				cv::moveWindow(window.first, x, 0);
				x += window.second.cols;
			}
		}



		void
		draw_indicators(cv::Mat & display)
		{
			cv::circle(display, {x_center, y_center}, 5, {0,255,0}, 2);

#ifdef WITH_DLIB
			// indicate modes
			if (detect)
			{
				cv::circle(display, {20,20}, 10, {255,0,0}, -2);
			}
			else if (train || images_train.size())
			{
				cv::circle(display, {20,20}, 10, {0,0,255}, -2);
			}
#endif //WITH_DLIB
			if (toggle_active)
			{
				for  (size_t i=0; i<active.size(); ++i)
				{
					int
						height = 50,
						width = 50,
						min_x = display.cols-width,
						max_x = display.cols,
						min_y = i*(height+5),
						max_y = min_y+height;
					cv::Mat hog;
					cv::Rect rect {min_x, min_y, width, height};

					if (active[i])
					{
#ifdef WITH_DLIB
						fhog_to_mat(detectors[i], hog);
#else //WITH_DLIB
						hog = display.clone();
#endif //WITH_DLIB
						width = hog.cols * height / hog.rows;
						min_x = display.cols-width;

						rect.width = width;
						rect.x = min_x;

						cv::resize(hog, hog, rect.size());
						cv::cvtColor(hog, hog, cv::COLOR_GRAY2BGR);
						hog.copyTo(display(rect));
					}

// 					cv::rectangle(display, rect, {255,167,87}, (active[i] ? -2 : 2));
					cv::rectangle(display, rect, {255,167,87}, 2);
					if (cntrs.size() > 0)
					{
						cv::Point2f cursor;
						contour_center(cntrs[0], cursor);
						cv::circle(display, cursor, 5, {0,128,255}, -2);
						//if (cursor.x > min_x and cursor.x < max_x and cursor.y > min_y and cursor.y < max_y)
						if (cursor.x > min_x && cursor.x < max_x && cursor.y > min_y && cursor.y < max_y)

						{
							//active[i] = not active[i];
							active[i] = !active[i];
							toggle_active = false;
						}
					}
				}
			}

			if (min_cntr_area)
			{
				int edge = std::sqrt(min_cntr_area);
				cv::rectangle(display, {10, 40}, {10+edge, 40+edge}, calibration_pixel, -2);
			}
		}



		void
		get_contours(cv::Mat const & mask)
		{
			std::vector<cv::Vec4i> hierarchy;
			cv::findContours(mask, cntrs, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
			std::sort(cntrs.begin(), cntrs.end(), compare_contour_area);
			if (set_min_cntr_area)
			{
				min_cntr_area = (cntrs.size() == 0 ? 0 : cv::contourArea(cntrs[0]));
				set_min_cntr_area = false;
			}
			if (min_cntr_area)
			{
				auto it = std::remove_if(
					cntrs.begin(),
					cntrs.end(),
					[&, this](std::vector<cv::Point> & cntr) -> bool {return (cv::contourArea(cntr) < this->min_cntr_area);}
				);
				cntrs.erase(it, cntrs.end());
			}
		}


#ifdef WITH_DLIB
		void
		hog_detect(cv::Mat const & frame, cv::Mat & display)
		{
			dlib::array2d<dlib::bgr_pixel> dlib_image;
			dlib::assign_image(dlib_image, dlib::cv_image<dlib::bgr_pixel>(frame));


			decltype(detectors) active_detectors;
			for (size_t i=0; i<detectors.size(); ++i)
			{
				if (active[i])
				{
					active_detectors.push_back(detectors[i]);
				}
			}


      std::vector<dlib::rectangle> rects = dlib::evaluate_detectors(active_detectors, dlib_image);
			for (auto dlib_rect : rects)
			{
				cv::Rect cv_rect = dlib_to_cv_rect(dlib_rect);
				cv::rectangle(display, cv_rect, {0,0,255}, 2);
			}
		}




		void
		clear_inactive()
		{
			std::cout << detectors.size() << " vs " << active.size() << std::endl;
			std::cout << "removing inactive detectors" << std::endl;
			auto it1 = std::remove_if(
				detectors.begin(),
				detectors.end(),
				//[&, this](dlib::object_detector<image_scanner_type> & dtr) -> bool {return not this->active.at(&dtr - this->detectors.data());}
				[&, this](dlib::object_detector<image_scanner_type> & dtr) -> bool {return !this->active.at(&dtr - this->detectors.data()); }

				);
			detectors.erase(it1, detectors.end());

			std::cout << "removing corresponding booleans" << std::endl;
			auto it2 = std::remove_if(
				active.begin(),
				active.end(),
				//[](bool actv) -> bool {return not actv;}
				[](bool actv) -> bool {return !actv; }
			);
			active.erase(it2, active.end());
		}



		void
		collect_training_images(cv::Mat const & frame)
		{
			/*
				Rate-limit to reduce number of training images. This should make
				the training faster. Sequential images are very similar so they
				should be redundant anyway.
			*/
			clock_t now = std::clock();
			double elapsed_sec = (double)(now - action_begin) / CLOCKS_PER_SEC;
			if (elapsed_sec >= 0.2)
			{
				action_begin = now;
				dlib::array2d<unsigned char> dlib_image;
				dlib::assign_image(dlib_image, dlib::cv_image<dlib::bgr_pixel>(frame));
				images_train.push_back(dlib_image);

				std::vector<dlib::rectangle> dlib_rects;
				std::transform(viewports.begin(), viewports.end(), std::back_inserter(dlib_rects), cv_to_dlib_rect);
				boxes_train.push_back(dlib_rects);

				std::cout << "collected training images: " << images_train.size() << std::endl;
			}
		}



		void
		train_detector()
		{
			std::cout << "training..." << std::endl;
			image_scanner_type scanner;
			scanner.set_detection_window_size(viewports[0].width, viewports[0].height);
			dlib::structural_object_detection_trainer<image_scanner_type> trainer(scanner);
			trainer.set_num_threads(4);
			trainer.set_c(1);
			trainer.be_verbose();
			trainer.set_epsilon(0.01);
			detectors.push_back(trainer.train(images_train, boxes_train));
			active.push_back(true);

			images_train.clear();
			boxes_train.clear();
			std::cout << "training done" << std::endl;
// 						dlib::draw_fhog(detector);
		}
#endif //WITH_DLIB


		bool
		draw_timer(cv::Mat const & display)
		{
			clock_t now = std::clock();
			double remaining_secs = conf_interval - ((double)(now - action_begin) / CLOCKS_PER_SEC);
			double fraction_remaining = std::fmax(remaining_secs / conf_interval, 0);
			int bar_width = fraction_remaining * (display.cols - 1);
			cv::rectangle(display, {0,display.rows-20}, {bar_width, display.rows}, {0,128,255}, -1);
			return (remaining_secs <= 0);
		}


		void
		configure_viewport(cv::Mat & display)
		{
			if (cntrs.size() > 1)
			{
				cv::Point2f pa;
				cv::Point2f pb;

				contour_center(cntrs[0], pa);
				contour_center(cntrs[1], pb);

				cv::circle(display, pa, 5, {0,128,255}, 2);
				cv::circle(display, pb, 5, {0,128,255}, 2);


				cv::Rect box {pa, pb};
				cv::rectangle(display, box, {0,255,0},2);

// 				int left = std::min(x_a, x_b);
// 				int width = std::abs(x_b-x_a);
// 				int bottom = std::min(y_a, y_b);
// 				int height = std::abs(y_b-y_a);
// 				cv::rectangle(display, {left, bottom}, {left+width, bottom+height}, {0,255,0}, 2);

				if (draw_timer(display))
				{
					viewports[0].width = box.width;
					viewports[0].height = box.height;
					vp_conf = false;
				}
			}
		}



		void
		configure_offset(cv::Mat const & display)
		{
			if (cntrs.size() > 1)
			{
				cv::Point2f pa;
				cv::Point2f pb;

				contour_center(cntrs[0], pa);
				contour_center(cntrs[1], pb);

				cv::circle(display, pa, 50, {0,255,0}, -2);
				cv::circle(display, pb, 5, {0,128,255}, 2);

				if (draw_timer(display))
				{
					vp_offset_x = pb.x - pa.x;
					vp_offset_y = pb.y - pa.y;
					pos_conf = false;
				}
			}
		}



		void
		draw_viewports(cv::Mat & display)
		{
			std::vector<cv::Scalar> colors {
				{255,50,128},
				{128,50,255},
				{50,128,255},
				{50,255,128},
				{128,255,50},
				{255,128,50}
			};
			size_t n = cntrs.size();
			if (n < 1)
			{
				return;
			}
			viewports.resize(n);
			std::fill_n(viewports.begin()+1, (n-1), viewports[0]);
			for (size_t i=0; i<n; ++i)
			{
				cv::Scalar color = colors[i%colors.size()];
				cv::Point2f vp_center;

				contour_center(cntrs[i], vp_center);
				cv::circle(display, vp_center, 5, color, -2);

				vp_center.x -= vp_offset_x;
				vp_center.y -= vp_offset_y;

				viewports[i].x = vp_center.x - viewports[i].width / 2;
				viewports[i].y = vp_center.y - viewports[i].height / 2;

				cv::rectangle(display, viewports[i], color, 2);
				cv::circle(display, vp_center, 5, color, 2);
				cv::drawContours(display, cntrs, i, color, 2);
			}
		}





		void
		handle_key(char key)
		{
			switch (key)
			{
				case 'q':
					running = false;
					break;
				case 'c':
					calibrate();
					break;
				case 'm':
					move_windows();
					break;
				case 'v':
					vp_conf = true;
					action_begin = std::clock();
					break;
				case 'a':
					set_min_cntr_area = true;
					break;
				case 'p':
					pos_conf = true;
					action_begin = std::clock();
					break;
#ifdef WITH_DLIB
				case 't':
					//train = not train;
					train = !train;
					action_begin = std::clock();
					break;
				case 'd':
					//detect = not detect;
					detect = !detect;
					break;
				case 'r':
					toggle_active = false;
					clear_inactive();
					break;
#endif //WITH_DLIB
				case 's':
					//toggle_active = not toggle_active;
					toggle_active = !toggle_active;
					break;
			}
		}



		void
		run()
		{
			cv::Mat frame, mask, display;
			std::vector<std::vector<cv::Point>> cntrs;
			cv::Moments cntr_moments;

			running = true;
			calibrated = false;
			set_min_cntr_area = false;

#ifdef WITH_DLIB
			images_train.clear();
			boxes_train.clear();
#endif //WITH_DLIB


			while (running)
			{
				if (! vc_prime.read(frame))
				{
					std::cerr << "Failed to read frame from capture device " << cam_prime << std::endl;
					break;
				}
				display = frame.clone();

				x_center = frame.cols / 2;
				y_center = frame.rows / 2;

				get_hsv_mask(frame, mask);
				get_contours(mask);

				draw_indicators(display);

				windows["frame"] = display;
				windows["mask"] = mask;

#ifdef WITH_DLIB
				if (detect)
				{
					hog_detect(frame, display);
				}
					else if (train)
				{
					draw_viewports(display);
					collect_training_images(frame);
				}
				else if (images_train.size() > 0)
				{
					train_detector();
				}
				else
#endif //WITH_DLIB
				if (vp_conf)
				{
					configure_viewport(display);
				}
				else if (pos_conf)
				{
					configure_offset(display);
				}
				else if (calibrated)
				{
					draw_viewports(display);
				}

#ifdef WITH_DLIB
#endif //WITH_DLIB


				cv::flip(display, display, 1);
				draw();
				handle_key(cv::waitKey(1) & 0xFF);
			}
		}
};
