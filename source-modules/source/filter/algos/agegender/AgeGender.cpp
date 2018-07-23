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

#include <algos/agegender/AgeGender.h>

#ifdef USE_CAFFE

void filter::algos::AgeGender::printResult(int _gender, int _age, std::vector<Dtype> _prob_gender,
                                           std::vector<Dtype> _prob_age)
{
	std::string gender;
	std::string age;
	bboxes_t boxes;

	// multiply 100 of each probability to make it as %
	std::for_each(_prob_gender.begin(), _prob_gender.end(), [&](Dtype& prob) { prob *= 100; });
	std::for_each(_prob_age.begin(), _prob_age.end(), [&](Dtype& prob) { prob *= 100; });

	// Convert int _gender into string
	switch (_gender)
	{
	case(0):
		gender = "Male";
		break;
	case(1):
		gender = "Female";
		break;
	}

	// Convert int _age into string
	switch (_age)
	{
	case(0):
		age = "0 - 2";
		break;
	case (1):
		age = "4 - 6";
		break;
	case (2):
		age = "8 - 13";
		break;
	case (3):
		age = "15 - 20";
		break;
	case (4):
		age = "25 - 32";
		break;
	case (5):
		age = "38 - 43";
		break;
	case (6):
		age = "48 - 53";
		break;
	case (7):
		age = "60 -";
		break;
	}

	// Print result
	std::cout << std::endl;
	std::cout << " =============== Prediction ===============" << std::endl << std::endl;
	std::cout << "1. Gender" << std::endl << std::endl;
	if (!_prob_gender.empty())
	{
		std::cout << " Male (" << _prob_gender[0] << " %)" << std::endl;
		std::cout << " Female (" << _prob_gender[1] << " %)" << std::endl;
		std::cout << " => " << gender << std::endl << std::endl;
	}
	std::cout << "2. Age " << std::endl << std::endl;
	std::cout << " 0 - 2 (" << _prob_age[0] << " %)" << std::endl;
	std::cout << " 4 - 6 (" << _prob_age[1] << " %)" << std::endl;
	std::cout << " 8 - 13 (" << _prob_age[2] << " %)" << std::endl;
	std::cout << " 15 - 20 (" << _prob_age[3] << " %)" << std::endl;
	std::cout << " 25 - 32 (" << _prob_age[4] << " %)" << std::endl;
	std::cout << " 38 - 43 (" << _prob_age[5] << " %)" << std::endl;
	std::cout << " 48 - 53 (" << _prob_age[6] << " %)" << std::endl;
	std::cout << " 60 - (" << _prob_age[7] << " %)" << std::endl;
	std::cout << " => " << age << std::endl << std::endl;

	std::cout << " ==========================================" << std::endl;
}

void filter::algos::AgeGender::startAgeGenderDetection()
{
	AgeGender* This = this;
	thr_server = new boost::thread([This]
	{
		while (This->isStart)
		{
			data::ImageData image;
			if (!This->imagesStack.trypop_until(image, 30))
				continue;

			int age = 0;

			int gender = 0;

			if (This->detectAges)
				age = This->detectAges->classify(image.getMat(), This->prob_age_vec);

			if (This->detectGenders)
				gender = This->detectGenders->classify(image.getMat(), This->prob_gender_vec);;


			bboxes_t bboxes = This->getBoxes(image.getMat(), gender, age, This->prob_gender_vec, This->prob_age_vec);

			This->boxes.push(bboxes);
		}
	});
}

filter::algos::AgeGender::bboxes_t filter::algos::AgeGender::getBoxes(cv::Mat frame, int _gender, int _age,
                                                                      vector<Dtype> _prob_gender,
                                                                      vector<Dtype> _prob_age)
{
	bboxes_t result;
	std::string gender;
	std::string age;

	// multiply 100 of each probability to make it as %
	std::for_each(_prob_gender.begin(), _prob_gender.end(), [&](Dtype& prob) { prob *= 100; });
	std::for_each(_prob_age.begin(), _prob_age.end(), [&](Dtype& prob) { prob *= 100; });
	

	gender = "M";
	// Convert int _gender into string
	switch (_gender)
	{
	case(0):
		gender = "Male";
		break;
	case(1):
		gender = "Female";
		break;
	}

	// Convert int _age into string
	switch (_age)
	{
	case(0):
		age = "?? - ??"; //0 - 2
		_prob_age[_age] = 0.;
		break;
	case (1):
		age = "?? - ??";
		_prob_age[_age] = 0.;
		break;
	case (2):
		age = "15 - 22";
		break;
	case (3):
		age = "23 - 27";
		break;
	case (4):
		age = "28 - 34";
		break;
	case (5):
		age = "35 - 42";
		break;
	case (6):
		age = "43 - 53";
		break;
	case (7):
		age = "60 - ??";
		break;
	}
	if (!_prob_gender.empty())
	{
		cout << " Male (" << _prob_gender[0] << " %)" << endl;
		cout << " Female (" << _prob_gender[1] << " %)" << endl;
		cout << " => " << gender << endl << endl;
	}
	/*cout << " Male (" << _prob_gender[0] << " %)" << endl;
	cout << " Female (" << _prob_gender[1] << " %)" << endl;*/
	// Print result
	/*cout << endl;
	cout << " =============== Prediction ===============" << endl << endl;
	cout << "1. Gender" << endl << endl;
	if (!_prob_gender.empty())
	{
		cout << " Male (" << _prob_gender[0] << " %)" << endl;
		cout << " Female (" << _prob_gender[1] << " %)" << endl;
		cout << " => " << gender << endl << endl;
	}
	cout << "2. Age " << endl << endl;
	cout << " 0 - 2 (" << _prob_age[0] << " %)" << endl;
	cout << " 4 - 6 (" << _prob_age[1] << " %)" << endl;
	cout << " 8 - 13 (" << _prob_age[2] << " %)" << endl;
	cout << " 15 - 20 (" << _prob_age[3] << " %)" << endl;
	cout << " 25 - 32 (" << _prob_age[4] << " %)" << endl;
	cout << " 38 - 43 (" << _prob_age[5] << " %)" << endl;
	cout << " 48 - 53 (" << _prob_age[6] << " %)" << endl;
	cout << " 60 - (" << _prob_age[7] << " %)" << endl;
	cout << " => " << age << endl << endl;

	cout << " ==========================================" << endl;*/
	cv::Rect rect;
	cv::Point offset;
	cv::Size wholesize;
	frame.locateROI(wholesize, offset);
	rect.x = offset.x;
	rect.y = offset.y;
	rect.width = frame.size().width;
	rect.height = frame.size().height;

	result.rectangles.push_back(rect);
	std::stringstream build_text;
	std::cout.precision(2);

	if (!_prob_gender.empty())
	{
		if (_prob_gender[0] < confidence_gender * 100.f && _prob_gender[1] < confidence_gender * 100.f)
			build_text << "Gender : " << "^_^";
		else {			
			build_text << gender << " (" << std::setprecision(2) << _prob_gender[_gender] << "%) ";
		}
	}

	if (!_prob_age.empty())
	{
		if (_prob_age[_age] < confidence_age * 100.f)
			build_text << " Age : " << "^_^";
		else
			build_text << " Age : " << age << " (" << std::setprecision(2) << _prob_age[_age] << "%) ";
	}
	
	result.names.push_back(build_text.str());
	return result;
}

#define cudaCheckError() {                                          \
 cudaError_t e  = cudaGetLastError();									\
 cudaError_t e2 = cudaDeviceSynchronize();									\
 if(e!=cudaSuccess) {												\
   std::stringstream error_msg;										\
error_msg << "Cuda failure " << __FILE__ << ":" << __LINE__ << " msg: " << cudaGetErrorString(e);           \
   throw HipeException(error_msg.str());							\
 }																	\
  if(e2!=cudaSuccess) {												\
   std::stringstream error_msg;										\
error_msg << "Cuda failure " << __FILE__ << ":" << __LINE__ << " msg: " << cudaGetErrorString(e2);           \
   throw HipeException(error_msg.str());							\
 }																	\
}

HipeStatus filter::algos::AgeGender::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat image = data.getMat();
	

	if (age_activation && !detectAges)
	{
		using namespace boost::filesystem;

		if (age_model_file == "" || age_weight_file == "")
		{
			throw HipeException("[Error] AgenGender::process - No input data found.");
		}

		bool pass = isFileExist(age_model_file) && isFileExist(age_weight_file) && isFileExist(mean_file);
		
		if (! pass)
		{
			throw HipeException("One or multiple file not found for AgeGender");
		}
		
		cudaCheckError();


		AgeNet* d = new AgeNet(age_model_file, age_weight_file, mean_file);

		d->initNetwork();

		detectAges.reset(d);
	}

	if (gender_activation && !detectGenders)
	{
		using namespace boost::filesystem;

		if (gender_model_file == "" || gender_weight_file == "")
		{
			throw HipeException("[Error] AgeGender::process - No input data found. gender_model_file or gender_weight_file");
		}

		isFileExist(gender_model_file);
		isFileExist(gender_weight_file);
		isFileExist(mean_file);
		
		GenderNet* d = new GenderNet(gender_model_file, gender_weight_file, mean_file);
		d->initNetwork();

		detectGenders.reset(d);
	}
	if (!isStart.exchange(true))
	{
		startAgeGenderDetection();
	}

	//FIXME std::vector<bbox_t> boxes;
	if (count_frame % skip_frame == 0)
	{
		if (!image.empty())
		{
			imagesStack.clear();
			imagesStack.push(image);

			bboxes_t r_boxes;
			if (boxes.trypop_until(r_boxes, 30)) // wait 30ms no more
			{
				saved_boxes = r_boxes;
			}
		}
	}
	
	data::ShapeData sd;
	if (!image.empty())
	{
		for (int i = 0; i < saved_boxes.rectangles.size(); i++)
		{
			cv::Scalar color(0, 113, 245);

			//cv::putText(image, saved_boxes.names[i], cv::Point(saved_boxes.rectangles[i].x, std::max<int>(saved_boxes.rectangles[i].y - 5, 0)),
			//            cv::HersheyFonts::FONT_HERSHEY_SIMPLEX, 1, color, 2);

			//cv::rectangle(image, saved_boxes.rectangles[i],
			//              color, 3);

			sd.add(saved_boxes.rectangles[i], color, saved_boxes.names[i]);
		}
	}
	else
	{
		saved_boxes.names.clear();
		saved_boxes.rectangles.clear();
	}

	PUSH_DATA(sd);

	return OK;
}
#endif //USE_CAFFE
