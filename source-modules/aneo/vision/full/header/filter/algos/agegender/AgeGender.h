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


#include <data/ImageData.h>
#include <data/ShapeData.h>

#include <filter/algos/agegender/AgeClassification.h>
#include <filter/algos/agegender/GenderClassification.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <boost/filesystem/operations.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
#ifdef USE_CAFFE
		class AgeGender : public filter::IFilter
		{
			class bboxes_t
			{
			public:
				std::vector<cv::Rect> rectangles;
				std::vector<std::string> names;
			};

			std::shared_ptr<AgeNet> detectAges;
			std::shared_ptr<GenderNet> detectGenders;
			std::vector<std::string> names;

			core::queue::ConcurrentQueue<data::ImageData> imagesStack;
			core::queue::ConcurrentQueue<bboxes_t> boxes;
			std::atomic<bool> isStart;
			boost::thread *thr_server;

			int skip_frame;
			int count_frame;
			bboxes_t saved_boxes;

			CONNECTOR(data::ImageData, data::ShapeData);

			REGISTER(AgeGender, ()), _connexData(data::INDATA)
			{
				age_model_file = "Data-light/AgeGender/model/deploy_age2.prototxt";
				gender_model_file = "Data-light/AgeGender/model/deploy_gender2.prototxt";

				age_weight_file = "Data-light/AgeGender/model/age_net.caffemodel";
				gender_weight_file = "Data-light/AgeGender/model/gender_net.caffemodel";
				mean_file = "Data-light/AgeGender/model/mean.binaryproto";


				skip_frame = 4;
				count_frame = 0;
				confidence_gender = 0.4;
				confidence_age = 0.4;
				gender_activation = true;
				age_activation = true;
				isStart = false;

				thr_server = nullptr;
			}

			REGISTER_P(std::string, age_model_file);
			REGISTER_P(std::string, gender_model_file);

			REGISTER_P(std::string, age_weight_file);
			REGISTER_P(std::string, gender_weight_file);

			REGISTER_P(std::string, mean_file);

			REGISTER_P(float, confidence_gender);
			REGISTER_P(float, confidence_age);

			REGISTER_P(bool, gender_activation);
			REGISTER_P(bool, age_activation);



			bboxes_t getBoxes(cv::Mat frame, int _gender, int _age, vector<Dtype> _prob_gender, vector<Dtype> _prob_age);

			// Probability vector
			std::vector<Dtype> prob_age_vec;
			std::vector<Dtype> prob_gender_vec;


			void printResult(int _gender, int _age, std::vector<Dtype> _prob_gender, std::vector<Dtype> _prob_age);

			void startAgeGenderDetection();

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

		ADD_CLASS(AgeGender, 
			gender_activation, age_activation, 
			confidence_gender, confidence_age,
			age_model_file, age_weight_file, 
			gender_model_file, gender_weight_file, mean_file);

#endif //USE_CAFFE
	}
}
