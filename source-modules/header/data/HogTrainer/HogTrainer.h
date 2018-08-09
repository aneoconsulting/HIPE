//@HIPE_LICENSE@
#pragma once
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <chrono>

#pragma warning(push, 0)      
#include <opencv2/opencv.hpp>
#if defined(USE_DLIB)
	//issue order of header for vector keyword call it before 
	#if defined(__ALTIVEC__)
	#include <dlib/simd.h>
	#endif
#include <dlib/opencv.h>
#include <dlib/svm_threaded.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/data_io.h>
#endif //USE_DLIB

#pragma warning(pop)

namespace data
{
	namespace hog_trainer
	{
		typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6> > image_scanner_type;


		/*!
		@brief Compare contours by area for sorting.
		@param a First contour to compare.
		@param b Second contour to compare.
		@return
		A boolean indicating if the area of the first contour is greater than the
		second.
		*/
		static
			bool
			compare_contour_area(std::vector<cv::Point> const & a, std::vector<cv::Point> const & b)
		{
			return cv::contourArea(a) > cv::contourArea(b);
		}


		/*!
		@brief Calculate the geometric center of a contour using its moments.
		@param cntr The input contour.
		@param point The point to update with the coordinates of the center.
		*/
		template <typename P>
		static
			void
			contour_center(std::vector<cv::Point> const & cntr, P & point)
		{
			cv::Moments cntr_moments;
			cntr_moments = cv::moments(cntr);
			point.x = cntr_moments.m10 / cntr_moments.m00;
			point.y = cntr_moments.m01 / cntr_moments.m00;
		}



		/*!
		@brief Convert a dlib fhog detector to an OpenCV image.
		@param detector The detector object.
		@param image The grayscale output image.
		*/
		static
			void
			fhog_to_mat(dlib::object_detector<image_scanner_type> const & detector, cv::Mat & image)
		{
			dlib::matrix<unsigned char> fhog = dlib::draw_fhog(detector);
			cv::Mat tmp(fhog.nr(), fhog.nc(), CV_8UC1);

			int i = 0;
			int j = 0;
			int cols = fhog.nc();
			unsigned char * row = tmp.ptr<unsigned char>(i);
			for (auto pixel : fhog)
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




		/*!
		@brief Convert an OpenCV rectangle to a dlib rectangle.
		@param r The OpenCV rectangle.
		@return A dlib rectangle.
		*/
		static
			inline
			dlib::rectangle
			cv_to_dlib_rect(cv::Rect r)
		{
			return dlib::rectangle((long)r.tl().x, (long)r.tl().y, (long)r.br().x - 1, (long)r.br().y - 1);
		}


		/*!
		@brief Convert a dlib rectangle to an OpenCV rectangle.
		@param r The dlib rectangle.
		@return The OpenCV rectangle.
		*/
		static
			inline
			cv::Rect
			dlib_to_cv_rect(dlib::rectangle r)
		{
			return cv::Rect(cv::Point2i(r.left(), r.top()), cv::Point2i(r.right() + 1, r.bottom() + 1));
		}




		/*!
		@brief A class for interactive training of dlib fhog detectors.
		*/
		template <typename IMAGE_SCANNER_TYPE>
		class HogTrainer
		{
		protected:
			static unsigned char const DEFAULT_HUE = 164; //pink
			static char const DELTA_H = 7, DELTA_S = 50, DELTA_V = 75;

			cv::Scalar hsv_min{ 90 - DELTA_H, 50, 40 };
			cv::Scalar hsv_max{ 90 + DELTA_H, 255, 255 };
			cv::Scalar center_pixel{ 0,0,0 };
			cv::Scalar calibration_pixel{ 0,0,0 };

			int x_center = 0, y_center = 0;

			unsigned char hue = DEFAULT_HUE;
			unsigned char center_h = 0, center_s = 0, center_v = 0;

			double configuration_interval = 5;
			std::chrono::system_clock::time_point action_begin;

			bool running = false;
			bool calibrated = false;
			bool save = false;
			bool set_min_cntr_area = false;
			double min_cntr_area = 0;

			std::vector<std::vector<cv::Point>> cntrs;

			std::vector<cv::Rect> viewports{ cv::Rect{ 0,0,150,150 } };
			int vp_offset_x = 0;
			int vp_offset_y = 175;

			bool vp_conf = false;
			bool pos_conf = false;

			std::map<const char *, cv::Mat> windows;


			bool detect = false;
			bool train = false;
			//! Interval to limit rate of training images
			double training_image_interval = 0.5;
			unsigned int n_training_threads = 4;
			double fhog_epsilon = 0.01;

			dlib::array<dlib::array2d<unsigned char>> images_train;
			std::vector<std::vector<dlib::rectangle>> boxes_train;

			std::vector<dlib::object_detector<IMAGE_SCANNER_TYPE>> detectors;
			std::vector<bool> active;
			bool toggle_active = false;

		public:
			HogTrainer(unsigned char h = DEFAULT_HUE) : hue{ h } {};
			~HogTrainer() { cv::destroyAllWindows(); }



			/*!
			@brief Move detectors from the internal vector to the target vector.
			@param target
			The destination vector.
			*/
			void
				move_get_detectors(std::vector<dlib::object_detector<IMAGE_SCANNER_TYPE>> target)
			{
				std::move(detectors.begin(), detectors.end(), std::back_inserter(target));
				detectors.resize(0);
			}


			/*!
			@brief Get a const reference to the current detectors.
			*/
			std::vector<dlib::object_detector<IMAGE_SCANNER_TYPE>> const &
				get_detectors()
			{
				return detectors;
			}



			/*!
			@brief Get the active detectors.
			*/
			void
				get_active_detectors(std::vector<dlib::object_detector<IMAGE_SCANNER_TYPE>> & active_detectors)
			{
				for (size_t i = 0; i < detectors.size(); ++i)
				{
					if (active[i])
					{
						active_detectors.push_back(detectors[i]);
					}
				}
			}



			/*!
			@brief Return the elapsed time since the action begin, in seconds.
			*/
			double
				get_elapsed_time()
			{
				std::chrono::system_clock::time_point t_now = std::chrono::system_clock::now();
				auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t_now - action_begin);
				return dt.count() / 1000.;
			}


			/*!
			@brief Reset the action timer.
			*/
			void
			reset_action_timer()
			{
				action_begin = std::chrono::system_clock::now();
			}


			/*!
			@brief Set the number of training images per second.
			*/
			void
			set_training_images_per_second(unsigned int ips)
			{
				training_image_interval = 1. / static_cast<double>(ips);
			}


			/*!
			@brief Set the number of training images per second.
			@param n The number of threads to use for fhog training.
			*/
			void
			set_number_training_threads(unsigned int n)
			{
				n_training_threads = n;
			}


			/*!
			@brief Set the epsilon value for the fhog trainer.
			@param eps The value to set.
			*/
			void
			set_fhog_epsilon(double eps)
			{
				fhog_epsilon = eps;
			}


			/*!
			@brief Set the configuration interval.
			@param eps The interval, in seconds.
			*/
			void
			set_configuration_interval(double secs)
			{
				configuration_interval = secs;
			}


			/*!
			@brief
			Set the min and max HSV ranges for filtering the target color. This
			uses the values of the pixel captured during the most recent capture
			action.
			*/
			void
			set_hsv_ranges()
			{
				hsv_min[1] = (center_s > DELTA_S ? center_s - DELTA_S : 0);
				hsv_max[1] = (255 - DELTA_S > center_s ? center_s + DELTA_S : 255);
				hsv_min[2] = (center_v > DELTA_V ? center_v - DELTA_V : 0);
				hsv_max[2] = (255 - DELTA_V > center_v ? center_v + DELTA_V : 255);
			}



			/*!
			@brief
			Calibrate the target color based on the center pixel of the last frame.
			*/
			void
			calibrate()
			{
				calibration_pixel = center_pixel;
				hue = center_h;
				set_hsv_ranges();
				calibrated = true;
			}



			/*!
			@brief
			Get the mask from the current frame based on the current target color.
			@param frame The input frame.
			@param mask The output grayscale image.

			*/
			void
				get_hsv_mask(cv::Mat const & frame, cv::Mat & mask)
			{
				cv::Mat hsv;
				int hue_shift = 90 - hue;

				cv::GaussianBlur(frame, hsv, { 5,5 }, 0);
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
					for (int i = 0; i < hsv.rows; ++i)
					{
						cv::Vec3b * row = hsv.ptr<cv::Vec3b>(i);
						for (int j = 0; j < hsv.cols; ++j)
						{
							row[j][0] = (row[j][0] + hue_shift) % 180;
						}
					}
				}

				// mask is a binary mask
				cv::inRange(hsv, hsv_min, hsv_max, mask);
				cv::GaussianBlur(mask, mask, { 5, 5 }, 0);
			}



			/*!
			@brief Draw all current windows.
			*/
			void
				draw()
			{
				for (auto const & window : windows)
				{
					cv::imshow(window.first, window.second);
				}
			}



			/*!
			@brief Align all windows in a row in the order in which they were drawn.
			*/
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



			/*!
			@brief
			Draw various indicators such as the circle around the center pixel and
			other shapes that indicate the current mode of operation.
			@param display
			The image on which to draw the shapes, intended for display.
			*/
			void
				draw_indicators(cv::Mat & display)
			{
				cv::circle(display, { x_center, y_center }, 5, { 0,255,0 }, 2);

				// indicate modes
				if (detect)
				{
					cv::circle(display, { 20,20 }, 10, { 255,0,0 }, -2);
				}
				else if (train || images_train.size())
				{
					cv::Scalar color {0,0,255};
					cv::circle(display, { 20,20 }, 10, color, -2);

					int scale = 2, thickness = 2, baseline, x = 40, y=20;
					std::string count =  std::to_string(images_train.size());
					cv::Size textsize = cv::getTextSize(count, CV_FONT_HERSHEY_PLAIN, scale, thickness, &baseline);
					y += textsize.height / 2;
					cv::putText(display, count, {x,y}, CV_FONT_HERSHEY_PLAIN, scale, color, thickness);
				}
				if (toggle_active)
				{
					for (size_t i = 0; i < active.size(); ++i)
					{
						int
							height = 50,
							width = 50,
							min_x = display.cols - width,
							max_x = display.cols,
							min_y = i*(height + 5),
							max_y = min_y + height;
						cv::Mat hog;
						cv::Rect rect{ min_x, min_y, width, height };

						if (active[i])
						{
							fhog_to_mat(detectors[i], hog);
							width = hog.cols * height / hog.rows;
							min_x = display.cols - width;

							rect.width = width;
							rect.x = min_x;

							cv::resize(hog, hog, rect.size());
							cv::cvtColor(hog, hog, cv::COLOR_GRAY2BGR);
							hog.copyTo(display(rect));
						}

						// 					cv::rectangle(display, rect, {255,167,87}, (active[i] ? -2 : 2));
						cv::rectangle(display, rect, { 255,167,87 }, 2);
						if (cntrs.size() > 0)
						{
							cv::Point2f cursor;
							contour_center(cntrs[0], cursor);
							cv::circle(display, cursor, 5, { 0,128,255 }, -2);
							if (cursor.x > min_x && cursor.x < max_x && cursor.y > min_y && cursor.y < max_y)
							{
								active[i] = !active[i];
								toggle_active = false;
							}
						}
					}
				}

				if (min_cntr_area)
				{
					int edge = std::sqrt(min_cntr_area);
					cv::rectangle(display, { 10, 40 }, { 10 + edge, 40 + edge }, calibration_pixel, -2);
				}
			}




			/*!
			@brief Detect contours in the mask.
			@param mask The mask.
			*/
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
						[&, this](std::vector<cv::Point> & cntr) -> bool {return (cv::contourArea(cntr) < this->min_cntr_area); }
					);
					cntrs.erase(it, cntrs.end());
				}
			}


			/*!
			@brief
			Evaluate all current fhog detectors on the current frame and draw the
			resulting rectangles on the display image.
			@param frame The input frame.
			@param display
			The image on which to draw the rectangles. It should be the the same
			size as frame and have 3 channels.
			*/
			void
				hog_detect(cv::Mat const & frame, cv::Mat & display)
			{
				dlib::array2d<dlib::bgr_pixel> dlib_image;
				dlib::assign_image(dlib_image, dlib::cv_image<dlib::bgr_pixel>(frame));


				decltype(detectors) active_detectors;
				for (size_t i = 0; i < detectors.size(); ++i)
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
					cv::rectangle(display, cv_rect, { 0,0,255 }, 2);
				}
			}




			/*!
			Clear detectors that have been marked inactive via the user interface.
			*/
			void
				clear_inactive()
			{
				std::cout << "removing inactive detectors" << std::endl;

				auto it1 = std::remove_if(
					detectors.begin(),
					detectors.end(),
					[&, this](dlib::object_detector<image_scanner_type> & dtr) -> bool {return !this->active.at(&dtr - this->detectors.data()); }
				);
				detectors.erase(it1, detectors.end());


				std::cout << "removing corresponding booleans" << std::endl;
				auto it2 = std::remove_if(
					active.begin(),
					active.end(),
					[](bool actv) -> bool {return !actv; }
				);
				active.erase(it2, active.end());
			}



			/*!
			@brief
			Collect training images. The collection is rate-limited to reduce the
			number of images and avoid dense sequences of similar images.
			@param The input frame.
			*/
			void
				collect_training_images(cv::Mat const & frame)
			{
				double elapsed_sec = get_elapsed_time();
				if (elapsed_sec >= training_image_interval)
				{
					reset_action_timer();
					dlib::array2d<unsigned char> dlib_image;
					dlib::assign_image(dlib_image, dlib::cv_image<dlib::bgr_pixel>(frame));
					images_train.push_back(dlib_image);

					std::vector<dlib::rectangle> dlib_rects;
					std::transform(viewports.begin(), viewports.end(), std::back_inserter(dlib_rects), cv_to_dlib_rect);
					boxes_train.push_back(dlib_rects);

					std::cout << "collected training images: " << images_train.size() << std::endl;
				}
			}



			/*!
			@brief Train the detector based on the current collected training images.
			*/
			void
				train_detector()
			{
				std::cout << "training..." << std::endl;
				image_scanner_type scanner;
				scanner.set_detection_window_size(viewports[0].width, viewports[0].height);
				dlib::structural_object_detection_trainer<image_scanner_type> trainer(scanner);
				trainer.set_num_threads(n_training_threads);
				trainer.set_c(1);
				trainer.be_verbose();
				trainer.set_epsilon(fhog_epsilon);

				detectors.push_back(trainer.train(images_train, boxes_train));

				active.push_back(true);

				images_train.clear();
				boxes_train.clear();
				std::cout << "training done" << std::endl;
				// 						dlib::draw_fhog(detector);
			}



			/*!
			@brief Draw a shrinking bar to indicate time remaining.
			@param display The image on which to draw the bar.
			@return True if the timer was drawn (i.e. the timer is still running).
			*/
			bool
				draw_timer(cv::Mat const & display)
			{
				double remaining_secs = configuration_interval - get_elapsed_time();
				if (remaining_secs > 0)
				{
					double fraction_remaining = std::fmax(remaining_secs / configuration_interval, 0);
					int bar_width = fraction_remaining * (display.cols - 1);
					cv::rectangle(display, { 0,display.rows - 20 }, { bar_width, display.rows }, { 0,128,255 }, -1);
					return true;
				}
				else
				{
					return false;
				}
			}


			/*!
			@brief
			Interactively configure the viewport size based on the two largest
			detected contours.
			@param display
			The image on which to draw the viewport and reference points.
			*/
			void
				configure_viewport_size_from_image(cv::Mat & display)
			{
				if (cntrs.size() > 1)
				{
					cv::Point2f pa;
					cv::Point2f pb;

					contour_center(cntrs[0], pa);
					contour_center(cntrs[1], pb);

					cv::circle(display, pa, 5, { 0,128,255 }, 2);
					cv::circle(display, pb, 5, { 0,128,255 }, 2);


					cv::Rect box{ pa, pb };
					cv::rectangle(display, box, { 0,255,0 }, 2);

					// 				int left = std::min(x_a, x_b);
					// 				int width = std::abs(x_b-x_a);
					// 				int bottom = std::min(y_a, y_b);
					// 				int height = std::abs(y_b-y_a);
					// 				cv::rectangle(display, {left, bottom}, {left+width, bottom+height}, {0,255,0}, 2);

					if (! draw_timer(display))
					{
						viewports[0].width = box.width;
						viewports[0].height = box.height;
						vp_conf = false;
					}
				}
			}



			/*!
			@brief
			Interactively configure the viewport positional offset based on the two
			largest detected contours.
			@param display
			The image on which to draw the viewport and reference points.
			*/
			void
				configure_viewport_offset_from_image(cv::Mat const & display)
			{
				if (cntrs.size() > 1)
				{
					cv::Point2f pa;
					cv::Point2f pb;

					contour_center(cntrs[0], pa);
					contour_center(cntrs[1], pb);

					cv::circle(display, pa, 50, { 0,255,0 }, -2);
					cv::circle(display, pb, 5, { 0,128,255 }, 2);

					if (! draw_timer(display))
					{
						vp_offset_x = pb.x - pa.x;
						vp_offset_y = pb.y - pa.y;
						pos_conf = false;
					}
				}
			}


			/*!
			@brief Configure viewport from parameters.
			@param width The viewport width.
			@param height The viewport height.
			@param x The x offset.
			@param y The y offset.
			*/
			void
				configure_viewport_from_parameters(int width, int height, int x, int y)
			{
				viewports[0].width = width;
				viewports[0].height = height;
				vp_offset_x = x;
				vp_offset_y = y;
			}




			/*!
			@brief Draw viewports for all currently detected contours.
			@param display The image on which to draw the viewports.
			*/
			void
				draw_viewports(cv::Mat & display)
			{
				std::vector<cv::Scalar> colors{
					{ 255,50,128 },
					{ 128,50,255 },
					{ 50,128,255 },
					{ 50,255,128 },
					{ 128,255,50 },
					{ 255,128,50 }
				};
				size_t n = cntrs.size();
				if (n < 1)
				{
					return;
				}
				viewports.resize(n);
				std::fill_n(viewports.begin() + 1, (n - 1), viewports[0]);
				for (size_t i = 0; i < n; ++i)
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





			/*!
			@brief Handle keys.

			a: Set a minimum contour area.
			c: Calibrate.
			d: Toggle detection.
			m: Move windows.
			p: Configue viewport positional offset.
			q: Quit running. Only applies to derived classes.
			r: Remove inactive filters.
			s: Toggle detector selection interface.
			t: Collect training images when toggled on, train detector when toggled off.
			v: Configure viewport size.
			*/
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
					reset_action_timer();
					break;
				case 'a':
					set_min_cntr_area = true;
					break;
				case 'p':
					pos_conf = true;
					reset_action_timer();
					break;
				case 't':
					train = !train;
					reset_action_timer();
					break;
				case 'd':
					detect = !detect;
					break;
				case 'r':
					toggle_active = false;
					clear_inactive();
					break;
				case 's':
					toggle_active = !toggle_active;
					break;
				default:
					break;
				}
			}



			/*!
			@brief Initialize and clear values prior to running in interactive mode.
			*/
			void
				init()
			{
				running = true;
				calibrated = false;
				set_min_cntr_area = false;

				images_train.clear();
				boxes_train.clear();
			}



			/*!
			@brief Process frame and all user interaction and display results.
			@param frame The input frame.
			*/
			void
				process_frame(cv::Mat & frame)
			{
				cv::Mat mask, display;
				std::vector<std::vector<cv::Point>> cntrs;

				// Mirror image for natural interaction.
				cv::flip(frame, frame, 1);
				display = frame.clone();

				x_center = frame.cols / 2;
				y_center = frame.rows / 2;

				get_hsv_mask(frame, mask);
				get_contours(mask);

				draw_indicators(display);

				windows["training"] = display;
				windows["mask"] = mask;

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
					if (vp_conf)
					{
						configure_viewport_size_from_image(display);
					}
					else if (pos_conf)
					{
						configure_viewport_offset_from_image(display);
					}
					else if (calibrated)
					{
						draw_viewports(display);
					}



				draw();
				handle_key(cv::waitKey(1) & 0xFF);
			}
		};




		/*!
		@brief HogTrainer derived class that manages the camera resource.
		*/
		template<typename IMAGE_SCANNER_TYPE>
		class HogTrainerWithCamera : public HogTrainer<IMAGE_SCANNER_TYPE>
		{
		private:
			//! @brief Primary camera number.
			int cam_prime = 0;
			cv::VideoCapture vc_prime;

		public:
			HogTrainerWithCamera(
				int cp = 0,
				unsigned char h = HogTrainer<IMAGE_SCANNER_TYPE>::DEFAULT_HUE
			) :
				HogTrainer<IMAGE_SCANNER_TYPE>{ h },
				cam_prime{ cp }
			{
				vc_prime = cv::VideoCapture(cam_prime);
			};

			~HogTrainerWithCamera()
			{
				vc_prime.release();
			}



			/*!
			@brief Capture images from camera and run interactively.
			*/
			void
				run()
			{
				cv::Mat frame;
				this->init();
				while (this->running)
				{
					if (!vc_prime.read(frame))
					{
						std::cerr << "Failed to read frame from capture device " << cam_prime << std::endl;
						break;
					}
					this->process_frame(frame);
				}
			}
		};
	}
}
