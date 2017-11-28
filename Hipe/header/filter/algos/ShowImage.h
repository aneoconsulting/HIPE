#pragma once
#include <filter/tools/RegisterTools.hpp>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <opencv2/highgui/highgui.hpp>

namespace filter
{
	namespace algos
	{
		/**
		 * \var ShowImage::waitkey
		 * [TODO] unused?
		 *
		 * \var ShowImage::wait
		 * Should we wait for the user's input to close the window?
		 *
		 * \var ShowImage::wait_ms
		 * The time to wait before the window is automatically closed. A negative value will let the window be permanently shown and wait for user input.
		 */

		 /**
		  **\todo
		  * \brief The ShowImage filter is used to show an image.
		  *
		  * The image will be shown in a dedicated window whose name will be the same as the one given to the filter graph node in the JSON request.
		  * The window can be automatically closed after a certain amount of time, or wait until the user do a keyboard input.
		  */
		class ShowImage : public IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(ShowImage, ()), _connexData(data::INOUT)
			{
				wait = false;
				wait_ms = 0;
				sequential = false;
				dedicated_window = true;
			}

			~ShowImage()
			{
				cv::destroyWindow(_name);
			}

			REGISTER_P(int, waitkey);
			REGISTER_P(bool, wait);
			REGISTER_P(int, wait_ms);
			REGISTER_P(bool, sequential);	// Wait for input after each image is shown
			REGISTER_P(bool, dedicated_window); // Should the images be shown in a unique window or in different ones?

		private:
			int count = 0;

		public:
			virtual std::string resultAsString() { return std::string("TODO"); };


			HipeStatus process()
			{
				while (!_connexData.empty()) // While i've parent data
				{
					data::ImageArrayData images = _connexData.pop();


					//Resize all images coming from the same parent
					for (auto &myImage : images.Array())
					{
						std::string windowName = _name;

						// Here, append image index if multiple windows should be used
						if (dedicated_window) windowName += "_" + std::to_string(++count);

						cv::namedWindow(windowName);
						cv::imshow(windowName, myImage);

						// Here, only wait for input if the user want to (sequential mode)
						if (sequential) waitKey();
					}

					// Here, only wait for input when all the images are shown (parallel mode)
					if (!sequential) waitKey();
				}

				return OK;
			}

			void dispose()
			{
				Model::dispose();

				
				// Destroy each window if in dedicated mode
				if (dedicated_window)
				{
					while (count > 0)
					{
						std::string windowName = _name;
						windowName += "_" + std::to_string(count--);
						cv::destroyWindow(windowName);
					}
				}
				// Destroy unique window if not
				else
				{
					std::string windowName = _name;
					cv::destroyWindow(windowName);
				}
			}

			void waitKey()
			{
				if (wait_ms <= 0 && wait)
				{
					std::cout << "Waiting for key..." << std::endl;
					cv::waitKey(0);
				}
				else if (wait_ms > 0)
				{
					cv::waitKey(wait_ms);
				}
			}
		};

		ADD_CLASS(ShowImage, waitkey, wait, wait_ms, sequential, dedicated_window);
	}
}
