#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#if OLD_YOLO
#include <yolo_v2_class.hpp>
#endif

#include <data/ImageData.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/dnn.hpp>
#pragma warning(pop)

namespace filter
{
	namespace algos
	{
		class ObjectRecognitionYolo : public filter::IFilter
		{
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

			int skip_frame;
			int count_frame;
			bboxes_t saved_boxes;

			CONNECTOR(data::ImageData, data::ShapeData);
		REGISTER(ObjectRecognitionYolo, ()), _connexData(data::INDATA)
			{
				cfg_filename = "NO FILE SET";
				weight_filename = "NO FILE SET";
				skip_frame = 4;
				count_frame = 0;
				confidenceThreshold = 0.8;
			}

		REGISTER_P(std::string, names_filename);
		REGISTER_P(std::string, cfg_filename);
		REGISTER_P(std::string, weight_filename);
		REGISTER_P(float, confidenceThreshold);

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
						throw HipeException("[Error] ObjectRecognitionYolo::process - No input data found.");
					}
					if (!detect)
					{
						using namespace boost::filesystem;

						if (cfg_filename == "" || weight_filename == "")
						{
							throw HipeException("[Error] ObjectRecognitionYolo::process - No input data found.");
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

			bboxes_t getBoxes(cv::Mat frame, cv::Mat detectionMat)
			{
				bboxes_t result;
				for (int i = 0; i < detectionMat.rows; i++)
				{
					const int probability_index = 5;
					const int probability_size = detectionMat.cols - probability_index;
					float* prob_array_ptr = &detectionMat.at<float>(i, probability_index);

					size_t objectClass = max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
					float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);

					if (confidence > confidenceThreshold)
					{
						float x_center = detectionMat.at<float>(i, 0) * frame.cols;
						float y_center = detectionMat.at<float>(i, 1) * frame.rows;
						float width = detectionMat.at<float>(i, 2) * frame.cols;
						float height = detectionMat.at<float>(i, 3) * frame.rows;
						Point p1(cvRound(x_center - width / 2), cvRound(y_center - height / 2));
						Point p2(cvRound(x_center + width / 2), cvRound(y_center + height / 2));
						Rect object(p1, p2);
						cv::String className = (objectClass < names.size()) ? cv::String(names[objectClass]) : cv::String(cv::format("unknown(%d)", objectClass));
						String label = format("%s: %.2f", className.c_str(), confidence);
						
						result.rectangles.push_back(object);
						result.names.push_back(label);
						//Scalar object_roi_color(0, 255, 0);
						//rectangle(frame, object, object_roi_color);


						
						/*int baseLine = 0;
						Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
						rectangle(frame, Rect(p1, Size(labelSize.width, labelSize.height + baseLine)),
						          object_roi_color, FILLED);
						putText(frame, label, p1 + Point(0, labelSize.height),
						        FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));*/
					}
				}
				return result;
			}

			HipeStatus process() override
			{
				data::ImageData data = _connexData.pop();
				cv::Mat image = data.getMat();
				bboxes_t boxes;

				if (!detect)
				{
					using namespace boost::filesystem;

					if (cfg_filename == "" || weight_filename == "")
					{
						throw HipeException("[Error] ObjectRecognitionYolo::process - No input data found.");
					}

					isFileExist(cfg_filename);
					isFileExist(weight_filename);
					//FIXME
					Darknet* d = new Darknet(cfg_filename, weight_filename);
					detect.reset(d);
					names = get_labels(names_filename);
				}

				//FIXME std::vector<bbox_t> boxes;
				if (count_frame % skip_frame == 0)
				{
					if (!image.data)
					{
						throw HipeException("[Error] ObjectRecognitionYolo::process - No input data found.");
					}

					//FIXME
					//Get NEW Boxes
					Mat inputBlob = cv::dnn::blobFromImage(image, 1 / 255.F, Size(416, 416), Scalar(), true, false);
					//Convert Mat to batch of images
					detect->net.setInput(inputBlob, "data"); //set the network input
					Mat detectionMat = detect->net.forward("detection_out"); //compute output

					boxes = getBoxes(image, detectionMat);
					saved_boxes = boxes;
				}
				else
				{
					//REUSE OLD BOSE
					boxes = saved_boxes;
				}

				data::ShapeData sd;
				for (int i = 0; i < boxes.rectangles.size(); i++)
				{
					cv::Scalar color(std::rand() % 255, std::rand() % 255, std::rand() % 255);

					//cv::putText(image, boxes.names[i], cv::Point(boxes.rectangles[i].x, boxes.rectangles[i].y),
					//            cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 1, color, 2);

					//cv::rectangle(image, boxes.rectangles[i],
					//              color, 3);

					sd.add(boxes.rectangles[i],color, boxes.names[i]);
				}

				PUSH_DATA(sd);

				return OK;
			}

			std::vector<std::string> get_labels(std::string filename)
			{
				std::ifstream ifs(filename);
				std::vector<std::string> lines;

				std::string line;
				while (std::getline(ifs, line))
				{
					// skip empty lines:
					if (line.empty())
						continue;

					lines.push_back(line);
				}

				return lines;
			}
		};

		ADD_CLASS(ObjectRecognitionYolo, names_filename, cfg_filename, weight_filename, confidenceThreshold) ;
	}
}
