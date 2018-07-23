//@HIPE_LICENSE@
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
				}
			};

			class bboxes_t
			{
			public:
				std::vector<cv::Rect> rectangles;
				std::vector<std::string> names;

			};

			std::shared_ptr<Darknet> detect;
			std::vector<std::string> names;


			int count_frame;
			bboxes_t saved_boxes;
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
				cfg_filename = "NO FILE SET";
				weight_filename = "NO FILE SET";
				skip_frame = 4;
				count_frame = 0;
				confidenceThreshold = 0.8;
				thr_server = nullptr;
				wait_ms = 30;
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

			bboxes_t getBoxes(cv::Mat frame, cv::Mat detectionMat);
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
