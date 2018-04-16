#include <filter/algos/detection/ObjectRecognitionYolo.h>

namespace filter
{
	namespace algos
	{
		void ObjectRecognitionYolo::startRecognition()
		{
			ObjectRecognitionYolo* This = this;

			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::ImageData image;
					if (!This->imagesStack.trypop_until(image, 30))
						continue;

					data::ShapeData bx = This->detectBoxes(image.getMat());

					if (This->shapes.size() != 0)
						This->shapes.clear();

					This->shapes.push(bx);
				}
			});
		}

		ObjectRecognitionYolo::bboxes_t ObjectRecognitionYolo::getBoxes(cv::Mat frame, cv::Mat detectionMat)
		{
			bboxes_t result;
			for (int i = 0; i < detectionMat.rows; i++)
			{
				const int probability_index = 5;
				const int probability_size = detectionMat.cols - probability_index;
				float* prob_array_ptr = &detectionMat.at<float>(i, probability_index);

				size_t objectClass = std::max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
				float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);

				if (confidence > confidenceThreshold)
				{
					float x_center = detectionMat.at<float>(i, 0) * frame.cols;
					float y_center = detectionMat.at<float>(i, 1) * frame.rows;
					float width = detectionMat.at<float>(i, 2) * frame.cols;
					float height = detectionMat.at<float>(i, 3) * frame.rows;
					cv::Point p1(cvRound(x_center - width / 2), cvRound(y_center - height / 2));
					cv::Point p2(cvRound(x_center + width / 2), cvRound(y_center + height / 2));
					cv::Rect object(p1, p2);
					cv::String className = (objectClass < names.size()) ? cv::String(names[objectClass]) : cv::String(cv::format("unknown(%d)", objectClass));
					cv::String label = cv::format("%s: %.2f", className.c_str(), confidence);

					result.rectangles.push_back(object);
					result.names.push_back(label);

				}
			}
			return result;
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

		data::ShapeData ObjectRecognitionYolo::detectBoxes(cv::Mat image)
		{
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
				cv::Mat inputBlob = cv::dnn::blobFromImage(image, 1 / 255.F, cv::Size(416, 416), cv::Scalar(), true, false);
				//Convert Mat to batch of images
				detect->net.setInput(inputBlob, "data"); //set the network input
				cv::Mat detectionMat = detect->net.forward("detection_out"); //compute output

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
				sd.add(boxes.rectangles[i], color, boxes.names[i]);
			}
			return sd;
		}

		HipeStatus ObjectRecognitionYolo::process()
		{
			if (!isStart.exchange(true))
			{
				startRecognition();
			}

			data::ImageData data = _connexData.pop();
			cv::Mat image = data.getMat();
			bboxes_t boxes;
			if (count_frame % skip_frame == 0)
			{
				if (imagesStack.size() != 0)
					imagesStack.clear();
				imagesStack.push(data);
			}
			count_frame++;
			data::ShapeData popShape;
			if (shapes.trypop_until(popShape, 30)) // wait 30ms no more
			{
				PUSH_DATA(popShape);
				tosend = popShape;
			}
			else if (tosend.empty())
			{
				PUSH_DATA(data::ShapeData());
			}
			else {

				PUSH_DATA(tosend);
			}

			return OK;
		}
	}
}