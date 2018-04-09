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
				age_model_file = "NO FILE SET";
				gender_model_file = "NO FILE SET";

				age_weight_file = "NO FILE SET";
				gender_weight_file = "NO FILE SET";

				skip_frame = 4;
				count_frame = 0;
				confidenceThreshold = 0.4;

				isStart = false;
			}

			REGISTER_P(std::string, age_model_file);
			REGISTER_P(std::string, gender_model_file);

			REGISTER_P(std::string, age_weight_file);
			REGISTER_P(std::string, gender_weight_file);

			REGISTER_P(std::string, mean_file);

			REGISTER_P(float, confidenceThreshold);


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

		ADD_CLASS(AgeGender, age_model_file, age_weight_file, gender_model_file, gender_weight_file, mean_file,
			confidenceThreshold) ;
#endif //USE_CAFFE
	}
}
