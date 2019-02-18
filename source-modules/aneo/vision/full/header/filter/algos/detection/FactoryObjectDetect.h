//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#if OLD_YOLO
#include <yolo_v2_class.hpp>
#endif
#include <fstream>
#include <boost/filesystem.hpp>
#include <data/ImageData.h>
#include <data/ShapeData.h>
#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <data/PatternData.h>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		class FactoryObjectDetect : public filter::IFilter
		{
			SET_NAMESPACE("vision/detection");

			class Darknet
			{
			public:
				cv::dnn::Net net;

				Darknet(std::string configurationName, std::string weightsModels)
				{
					net = cv::dnn::readNetFromDarknet(configurationName, weightsModels);
					net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
					net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
				}
			};

			

			std::shared_ptr<Darknet> detect;
			std::vector<std::string> names;


			int count_frame;
			data::ShapeData saved_boxes;
			boost::thread *thr_server;
			std::atomic<bool> isStart;		//<! [TODO] Is the thread running?
			core::queue::ConcurrentQueue<data::ImageData> imagesStack;	//<! [TODO] The queue containing the frames to process.
			core::queue::ConcurrentQueue<data::ShapeData> shapes;			//<! [TODO] The shapes of the found faces.
			data::ShapeData tosend;			//<! The image containing the drawn facial landmarks to output to the ConnexData port.
			int count;


			void startRecognition();

			CONNECTOR(data::ImageData, data::ShapeData);
			REGISTER(FactoryObjectDetect, ()), _connexData(data::INDATA)
			{
				cfg_filename = "Data-light/Yolo Detection/yolo-mg21.cfg";
				weight_filename = "Data-light/Yolo Detection/yolo-mg21_1300.weights";
				names_filename = "Data-light/Yolo Detection/yolo-mg21_2100.weights";
				skip_frame = 4;
				count_frame = 0;
				confidenceThreshold = 0.8;
				thr_server = nullptr;
				wait_ms = 300;
				count = 0;
				isStart = false;
			}

			REGISTER_P(std::string, names_filename);
			REGISTER_P(std::string, cfg_filename);
			REGISTER_P(std::string, weight_filename);
			REGISTER_P(float, confidenceThreshold);
			REGISTER_P(int, skip_frame);
			REGISTER_P(int, wait_ms);

#ifdef OLD_YOLO
			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				count_frame++;
				std::vector<bbox_t> boxes;
				if (count_frame % skip_frame == 0)
				{
					if (!image.data)
					{
						throw HipeException("[Error] FactoryObjectDetect::process - No input data found.");
					}
					if (!detect)
					{
						using namespace boost::filesystem;

						if (cfg_filename == "" || weight_filename == "")
						{
							throw HipeException("[Error] FactoryObjectDetect::process - No input data found.");
						}

						isFileExist(cfg_filename);
						isFileExist(weight_filename);

						Detector * d = new Detector(cfg_filename, weight_filename, 0);
						detect.reset(d);
						names = get_labels(names_filename);
					}
					boxes = detect->detect(image);
					saved_boxes = boxes;


				}
				else
				{
					boxes = saved_boxes;
				}

				for (int i = 0; i < boxes.size(); i++)
				{
					cv::Scalar color(std::rand() % 255, std::rand() % 255, std::rand() % 255);

					cv::putText(image, names[boxes[i].obj_id % names.size()], cv::Point(boxes[i].x, boxes[i].y),
						cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 1, color, 2);

					cv::rectangle(image, cv::Rect(boxes[i].x, boxes[i].y, boxes[i].w, boxes[i].h),
						color, 3);
				}

				PUSH_DATA(data::ImageData(image));

				return OK;
			}
#endif //OLD_YOLO

			data::ShapeData getBoxes(cv::Mat frame, const std::vector<cv::Mat> & outs);

			data::ShapeData detectBoxes(cv::Mat image);
			HipeStatus process() override;
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

		ADD_CLASS(FactoryObjectDetect, names_filename, cfg_filename, weight_filename, confidenceThreshold, skip_frame, wait_ms);
	}
}
