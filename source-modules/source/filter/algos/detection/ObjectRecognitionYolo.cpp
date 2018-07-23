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

#include <filter/algos/detection/ObjectRecognitionYolo.h>

namespace filter
{
	namespace algos
	{
		static void showImage(const cv::Mat& image, std::string name, bool shouldDestroy, int waitTime)
		{
			cv::namedWindow(name);
			cv::imshow(name, image);
			if (waitTime >= 0) cv::waitKey(waitTime);
			if (shouldDestroy) cv::destroyWindow(name);
		}

		void ObjectRecognitionYolo::startRecognition()
		{
			ObjectRecognitionYolo* This = this;

			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::ImageData image;
					try
					{
						if (!This->imagesStack.trypop_until(image, 2))
							continue;
						This->count_frame++;
						if (This->skip_frame == 0 || (This->count_frame % This->skip_frame) == 0)
						{
							cv::Mat imgMat = image.getMat();


							data::ShapeData bx = This->detectBoxes(imgMat);

							if (This->shapes.size() != 0)
								This->shapes.clear();

							This->shapes.push(bx);
							This->count_frame = 0;
						}
						This->imagesStack.clear();
					}
					catch (HipeException& e)
					{
						std::cerr << "ERROR : Fail to start recognition msg : " << e.what() << std::endl;
						This->isStart = false;
					}
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

		static std::vector<std::string> get_labels(std::string filename)
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
			if (hasError)
			{
				return data::ShapeData();
			}

			if (!detect)
			{
				using namespace boost::filesystem;

				try
				{
					if (cfg_filename == "" || weight_filename == "")
					{
						throw HipeException("[Error] ObjectRecognitionYolo::process - No input data found.");
					}
					
					if (!isFileExist(cfg_filename)) throw HipeException("Cannot find file " + cfg_filename);
					if (!isFileExist(weight_filename)) throw HipeException("Cannot find file " + weight_filename);
					
					//FIXME
					Darknet* d = new Darknet(cfg_filename, weight_filename);
					detect.reset(d);
					names = get_labels(names_filename);
				}
				catch (std::invalid_argument& e)
				{
					std::cerr << "ERROR : Fail to init recognition msg : " << e.what() << std::endl;
					isStart = false;
					hasError = true;
					throw HipeException(e.what());
				}
				catch (HipeException& e)
				{
					std::cerr << "ERROR : Fail to init recognition msg : " << e.what() << std::endl;
					isStart = false;
					hasError = true;
					throw e;
				}

				
			}

			{
				if (!image.data)
				{
					throw HipeException("[Error] ObjectRecognitionYolo::process - No input data found.");
				}

				//FIXME
				//Get NEW Boxes
				//showImage(image, "Debug", false, 3);
				cv::Mat inputBlob;
				inputBlob = cv::dnn::blobFromImage(image, 1 / 255.F, cv::Size(416, 416), cv::Scalar(), true, false);
				//Convert Mat to batch of images
				detect->net.setInput(inputBlob, "data"); //set the network input
				//if (inputBlob.rows > 0 && inputBlob.cols > 0)
				{
					cv::Mat detectionMat = detect->net.forward("detection_out"); //compute output
					boxes = getBoxes(image, detectionMat);
					saved_boxes = boxes;
				}

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
			if (_connexData.size() == 0)
			{
				PUSH_DATA(data::ShapeData());

				return OK;
			}

			if (!isStart.exchange(true))
			{
				startRecognition();
			}

			data::ImageData data = _connexData.pop();
			cv::Mat image = data.getMat();
			bboxes_t boxes;


			imagesStack.push(data);


			data::ShapeData popShape;
			if (shapes.trypop_until(popShape, wait_ms)) // wait 30ms no more
			{
				PUSH_DATA(popShape);
				tosend = popShape;
			}
			else if (tosend.empty())
			{
				PUSH_DATA(data::ShapeData());
			}
			else
			{
				PUSH_DATA(tosend);
			}

			return OK;
		}
	}
}
