#pragma once


#pragma once
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/PatternData.h>
#include "core/queue/ConcurrentQueue.h"

namespace filter
{
	namespace algos
	{
		/**
		 * \todo
		 * \brief Use this method as a mouse callback to know where the user clicked and define the areas he selected.
		 * \param event The type of the mouse event that occured.
		 * \param x The x position (relative to the targeted window) where the user clicked.
		 * \param y The y position (relative to the targeted window) where the user clicked.
		 * \param f [TODO]
		 * \param cvCropperData The struture used to store the information regarding what the user did while clicking.
		 */
		void cropper_mouse_call(int event, int x, int y, int f, void* cvCropperData);

		/**
		 * \todo
		 * \brief The Cropper filter will let the user select regions of interest in an image.
		 * 
		 *  The filter will extract the relative data of the selected regions of interest and return them as a PatternData object.
		 *  The Cropper filter runs in a separate thread.
		 *  
		 *  \var Cropper::wait
		 *  [TODO]
		 */
		class FILTER_EXPORT Cropper : public IFilter
		{
		public:
			/**
			 * \brief The CVCropperData class stores the data relative to where the user clicked on an OpenCV window.
			 */
			class CVCropperData
			{
			public:
				std::atomic<bool> clicked;			//<! Does the user clicked on the window?
				std::atomic<bool> drawing;			//<! Does the user clicked and is dragging the mouse?
				int index;							//<! [TODO] Index used to know the number of times the user selected a region in the image.
				cv::String windowName;				//<! The name of the window on which the user is clicking.
				std::vector<cv::Point> rectangle;	//<! [TODO] The list of the zones positions the user selected on the window.
				cv::Mat sandBoxImage;				//<! The part of the image (region of interest) extracted from the area the user selected with mouse input.
				cv::Mat sandBoxImage_backup;		//<! A backup of the sandBoxImage data.
				data::PatternData pattern;			//<! The PatternData object referencing the data of all the regions of interest the user selected.
				std::atomic<bool> started;			//<! [TODO] Does the user started to interact with the window?

				core::queue::ConcurrentQueue<std::vector<cv::Point>> arrayCrop; //<! [TODO]

			public:
				/**
				 * \todo
				 * \brief CVCropperData constructor with PatternData parameter. Will add data to it.
				 * \param pat PatternData object to start from.
				 */
				CVCropperData(const data::PatternData & pat) : clicked(false), drawing(false), index(0), pattern(pat)
				{
					started = false;
				}

				/**
				 * \brief CVCropperData constructor with image parameter. [TODO]
				 * \param image [TODO]
				 */
				CVCropperData(data::ImageData &image) : clicked(false), drawing(false), index(0), pattern(image)
				{
					started = false;
				}

				/**
				 * \brief CVCropperData default constructor. [TODO]
				 */
				CVCropperData() : clicked(false), drawing(false), index(0)
				{
					started = false;
				}

				/**
				 * \brief Overloaded insertion operator used to overwrite the CVCropperData's PatternData field's request image.
				 * \param inputImage The image which will be used to overwrite the PatternData request image.
				 * \return Returns a reference to the CVCropperData object.
				 */
				CVCropperData &operator<<(data::ImageData &inputImage)
				{
					pattern << inputImage;

					return *this;
				}

				/**
				 * \brief Overloaded insertion operator used to overwrite the CVCropperData's PatternData afield's request image.
				 * \param inputImage The image which will be used to overwrite the PatternData request image.
				 * \return Returns a reference to the CVCropperData object.
				 */
				CVCropperData &operator<<(cv::Mat inputImage)
				{

					pattern << data::ImageData(inputImage);

					return *this;
				}

			};

			std::shared_ptr<data::PatternData> _pattern;		//<! [TODO]
			std::shared_ptr<boost::thread> _task;				//<! [TODO]
			std::shared_ptr<CVCropperData> _cvCropperData;		//<! [TODO]


			CONNECTOR(data::ImageData, data::PatternData);



			REGISTER(Cropper, ()), _connexData(data::INDATA)
			{
				_cvCropperData = std::make_shared<CVCropperData>();
				wait = false;
			}

			REGISTER_P(bool, wait);
			~Cropper()
			{

			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		private:

			/**
			 * \brief Wrapper method to let the user select areas in an image.
			 * \param cvCropperData The CVCropperData object to populate from user input.
			 */
			void windowTask(std::shared_ptr<filter::algos::Cropper::CVCropperData> cvCropperData)
			{
				cv::namedWindow(_name, CV_WINDOW_AUTOSIZE | CV_WINDOW_KEEPRATIO);
				cv::setMouseCallback(_name, cropper_mouse_call, &_cvCropperData);

				while (_cvCropperData->started) {
					cv::imshow(_name, cvCropperData->sandBoxImage);
					auto ret = cv::waitKey(30);

					if (ret == 27) {
						cv::destroyWindow(_name);
						break;
					}
					if (ret == 13 || ret == ' ') // Return or space pressed
					{
						std::cout << "Keyboard has been pressed : " << ret << std::endl;



						cv::Mat patImage;
						cvCropperData->sandBoxImage_backup.copyTo(patImage);

						//Populate SquareCrops
						cvCropperData->pattern.crops().addPair(cvCropperData->rectangle, patImage);

						cvCropperData->arrayCrop.push(cvCropperData->rectangle);

						cv::destroyWindow(_name);
						break;
					}
					if (ret > 0)
					{
						std::cout << "unkonwn Keyboard has been pressed : " << ret << std::endl;
					}
				}
			}

			void CropperAreaDrawer()
			{
				//If the user doesn't yet click then refresh the sandbox
				if (!_cvCropperData->clicked)
				{
					_cvCropperData->pattern.imageRequest().getMat().copyTo(_cvCropperData->sandBoxImage);
				}
				if (!_task)
				{
					_cvCropperData->started = true;
					_cvCropperData->windowName = _name;

					_cvCropperData->clicked = false;

					_task = std::make_shared<boost::thread>(boost::bind(&Cropper::windowTask, this, _cvCropperData));
				}
				
				
			
			}

		public:


			HipeStatus process()
			{
				if (_pattern)
				{
					data::ImageData image = _connexData.pop();
					(*_pattern) << image;
					data::PatternData pattern(image);
					(*_pattern).copyTo(pattern);
					PUSH_DATA(pattern);

					return OK;
				}


				if (_connexData.size() == 0)
					throw HipeException("There is no data to display coming from the parent node [NAME]");

				data::ImageData image = _connexData.pop();
				auto myImage = image.getMat();
				if (myImage.rows <= 0 || myImage.cols <= 0)
					throw HipeException("Image to show doesn't data");



				*(_cvCropperData) << myImage;

				CropperAreaDrawer();
				std::vector<cv::Point> rect;
				if (wait)
				{
					_cvCropperData->arrayCrop.wait_and_pop(rect);
					PUSH_DATA(_cvCropperData->pattern);
					_pattern = std::make_shared<data::PatternData>(_cvCropperData->pattern);
				}
				else if (_cvCropperData->arrayCrop.trypop_until(rect, 10) == false)
				{
					data::PatternData pattern(image);

					PUSH_DATA(pattern);

				}
				else
				{
					PUSH_DATA(_cvCropperData->pattern);
					_pattern = std::make_shared<data::PatternData>(_cvCropperData->pattern);
				}

				

				
				
				

				return OK;
			}

			void dispose()
			{
				Model::dispose();
				if (_task)
					_cvCropperData->started = false;
				cv::destroyWindow(_name);
			}
		};

		ADD_CLASS(Cropper, wait);
	}
}
