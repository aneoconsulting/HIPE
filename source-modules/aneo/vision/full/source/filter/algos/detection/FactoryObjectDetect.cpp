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

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <filter/algos/detection/FactoryObjectDetect.h>
#include <glog/logging.h>


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

		//
		//		// Draw the predicted bounding box
		//void drawPred(int classId, float conf, int left, int top, int right, int bottom, cv::Mat& frame)
		//{
		//    //Draw a rectangle displaying the bounding box
		//    cv::rectangle(frame, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(0, 0, 255));
		//     
		//    //Get the label for the class name and its confidence
		//    std::string label = cv::format("%.2f", conf);
		//    if (!classes.empty())
		//    {
		//        CV_Assert(classId < (int)classes.size());
		//        label = classes[classId] + ":" + label;
		//    }
		//     
		//    //Display the label at the top of the bounding box
		//    int baseLine;
		//	cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
		//    top = max(top, labelSize.height);
		//    putText(frame, label, cv::Point(left, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,255,255));
		//}
		// Remove the bounding boxes with low confidence using non-maxima suppression
		data::ShapeData postprocess(cv::Mat& frame, const std::vector<cv::Mat>& outs, std::vector<std::string> classes,
		                            float confThreshold, float nmsThreshold)
		{
			std::vector<int> classIds;
			std::vector<float> confidences;
			std::vector<cv::Rect> boxes;

			for (size_t i = 0; i < outs.size(); ++i)
			{
				// Scan through all the bounding boxes output from the network and keep only the
				// ones with high confidence scores. Assign the box's class label as the class
				// with the highest score for the box.
				float* data = (float*)outs[i].data;
				for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
				{
					cv::Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
					cv::Point classIdPoint;
					double confidence;
					// Get the value and location of the maximum score
					minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
					if (confidence > confThreshold)
					{
						int centerX = (int)(data[0] * frame.cols);
						int centerY = (int)(data[1] * frame.rows);
						int width = (int)(data[2] * frame.cols);
						int height = (int)(data[3] * frame.rows);
						int left = centerX - width / 2;
						int top = centerY - height / 2;

						classIds.push_back(classIdPoint.x);
						confidences.push_back((float)confidence);
						boxes.push_back(cv::Rect(left, top, width, height));
					}
				}
			}

			// Perform non maximum suppression to eliminate redundant overlapping boxes with
			// lower confidences
			std::vector<int> indices;
			data::ShapeData shape;
			cv::dnn::NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
			for (size_t i = 0; i < indices.size(); ++i)
			{
				int idx = indices[i];
				cv::Rect box = boxes[idx];
				//drawPred(classIds[idx], confidences[idx], box.x, box.y,
				//         box.x + box.width, box.y + box.height, frame);

				shape.RectsArray().push_back(box);
				shape.IdsArray().push_back(classes[classIds[idx]]);
			}

			return shape;
		}

		void FactoryObjectDetect::startRecognition()
		{
			FactoryObjectDetect* This = this;

			thr_server = new boost::thread([This]
			{
				while (This->isStart)
				{
					data::ImageData image;
					if (!This->imagesStack.trypop_until(image, 1))
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
			});
		}

		data::ShapeData FactoryObjectDetect::getBoxes(cv::Mat frame, const std::vector<cv::Mat> & outs)
		{
			data::ShapeData shapes = postprocess(frame, outs, names, confidenceThreshold, 0.4);
			
			return shapes;
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

		// Get the names of the output layers
		std::vector<cv::String> getOutputsNames(const cv::dnn::Net& net)
		{
			static std::vector<cv::String> names;
			if (names.empty())
			{
				//Get the indices of the output layers, i.e. the layers with unconnected outputs
				std::vector<int> outLayers = net.getUnconnectedOutLayers();

				//get the names of all the layers in the network
				std::vector<cv::String> layersNames = net.getLayerNames();

				// Get the names of the output layers in names
				names.resize(outLayers.size());
				for (size_t i = 0; i < outLayers.size(); ++i)
					names[i] = layersNames[outLayers[i] - 1];
			}
			return names;
		}

		data::ShapeData FactoryObjectDetect::detectBoxes(cv::Mat image)
		{
			if (!detect)
			{
				using namespace boost::filesystem;

				if (cfg_filename == "" || weight_filename == "")
				{
					throw HipeException("[Error] FactoryObjectDetect::process - No input data found.");
				}
				try
				{
					isFileExist(cfg_filename);
					isFileExist(weight_filename);
				}
				catch (std::exception& e)
				{
					std::stringstream err;
					err << "Cannot find file " << cfg_filename << " or " << weight_filename << std::endl;
					err << "WorkingDirectory : " << GetCurrentWorkingDir();

					LOG(ERROR) << err.str();
					throw HipeException(err.str());
				}

				//FIXME
				Darknet* d = new Darknet(cfg_filename, weight_filename);
				detect.reset(d);
				names = get_labels(names_filename);
			}

			//FIXME std::vector<bbox_t> boxes;
			//if (count_frame % skip_frame == 0)
			{
				if (!image.data)
				{
					throw HipeException("[Error] FactoryObjectDetect::process - No input data found.");
				}

				//FIXME
				//Get NEW Boxes
				//showImage(image, "Debug", false, 3);
				cv::Mat inputBlob;
				inputBlob = cv::dnn::blobFromImage(image, 1 / 255.F, cv::Size(416, 416), cv::Scalar(), true, false);
				//Convert Mat to batch of images
				detect->net.setInput(inputBlob); //set the network input
				//if (inputBlob.rows > 0 && inputBlob.cols > 0)
				{
					std::vector<cv::Mat> outs;

					detect->net.forward(outs, getOutputsNames(detect->net)); //compute output
					data::ShapeData boxes = getBoxes(image, outs);
					saved_boxes = boxes;
				}
			}

			data::ShapeData sd;
			saved_boxes.copyTo(sd);


			return sd;
		}

		HipeStatus FactoryObjectDetect::process()
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

			data::ShapeData boxes;

			while (!_connexData.empty())
			{
				data::ImageData data = _connexData.pop();
				//cv::Mat image = data.getMat();

				imagesStack.push(data);
			}

			data::ShapeData popShape;
			if (shapes.trypop_until(popShape, wait_ms)) // wait 30ms no more
			{
				PUSH_DATA(popShape);
				tosend = popShape;
				count = 0;
			}
			else if (tosend.empty())
			{
				PUSH_DATA(data::ShapeData());
			}
			else
			{
				count++;
				if (skip_frame == 0 || count < 4 * skip_frame)
				{
					PUSH_DATA(tosend);
				}
				else
				{
					data::ShapeData shape_data = data::ShapeData();
					tosend = shape_data;
					count = 0;
				}
			}

			return OK;
		}
	}
}
