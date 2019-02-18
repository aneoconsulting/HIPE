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

#include <algos/agegender/GenderClassification.h>
#include <algos/agegender/Caffe_os_deps.h>

#pragma warning(push, 0)
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

#pragma warning(pop)

#ifdef USE_CAFFE
/*
Method : initNetwork

Read network architecture Frmo model_file and apply weights from weight_file
*/
void GenderNet::initNetwork()
{
	NetParameter net_param;
	ReadNetParamsFromTextFileOrDie(this->model_file, &net_param);

	this->gender_net = std::make_shared<Hipe_net>(net_param);
	gender_net->CopyTrainedLayersFrom(this->weight_file);
}

/*
Method : getMeanImgFromMeanFile

Get cv::Mat from mean_file.
This work goes through the steps below.

1. Get BlobProto from mean_file.
2. Make Blob using BlobProto
3. Make three cv::Mat(one Mat per channel) using Blob
4. Merge three channels Mat.
5. Get cv::Scalar(one value per channel) with the mean values of each channel(mean value of R channel,... and so on).
6. Make mean img with this scalar.

Param :
1. _mean_img : [output] mean image 
*/
void GenderNet::getMeanImgFromMeanFile(Mat& _mean_img)
{
	BlobProto blob_proto;
	ReadProtoFromBinaryFile(this->mean_file.c_str(), &blob_proto);

	Hipe_blob mean_blob;
	mean_blob.FromProto(blob_proto);

	vector<Mat> channels;
	Dtype * mean_data = mean_blob.mutable_cpu_data();

	for (int i = 0; i < mean_blob.channels(); i++)
	{
		Mat channel(mean_blob.width(), mean_blob.height(), CV_8UC1, mean_data);
		channels.push_back(channel);
		mean_data += mean_blob.width() * mean_blob.height();
	}

	Mat rgb_mean_img;
	cv::merge(channels, rgb_mean_img);

	cv::Scalar channel_mean = cv::mean(rgb_mean_img);
	_mean_img = cv::Mat(rgb_mean_img.rows, rgb_mean_img.cols, rgb_mean_img.type(), channel_mean);
}

/*
Method : makeBlobVecWithCroppedImg

This is a kind of useful technique to make performance better.

1. Resize image to 256 x 256
2. Subtract mean image
3. Get 5(left top, right top, left bottom, right bottom, center) cropped images with size of 227 x 227.
4. Make Blob of those 5 images.
5. Make Blob vector.

Param :
1. _img : [in] input image
2. _blob_vec : [out] output blob vector made by five cropped image of _img.
*/
void GenderNet::makeBlobVecWithCroppedImg(Mat _img, vector<Hipe_blob *>& _blob_vec)
{
	Mat resized_img;
	Mat mean_img;
	Mat normalized_img;

	cv::resize(_img, resized_img, cv::Size(256, 256));

	this->getMeanImgFromMeanFile(mean_img);
	subtract(resized_img, mean_img, normalized_img);

	Mat lt_img = normalized_img(cv::Rect(0, 0, 227, 227));
	Mat rt_img = normalized_img(cv::Rect(29, 0, 227, 227));
	Mat lb_img = normalized_img(cv::Rect(0, 29, 227, 227));
	Mat rb_img = normalized_img(cv::Rect(29, 29, 227, 227));
	Mat ctr_img = normalized_img(cv::Rect(14, 14, 227, 227));

	Hipe_blob * lt_blob = new Hipe_blob(1, 3, 227, 227);
	Hipe_blob * rt_blob = new Hipe_blob(1, 3, 227, 227);
	Hipe_blob * lb_blob = new Hipe_blob(1, 3, 227, 227);
	Hipe_blob * rb_blob = new Hipe_blob(1, 3, 227, 227);
	Hipe_blob * ctr_blob= new Hipe_blob(1, 3, 227, 227);

	TransformationParameter trans_param;
	DataTransformer<Dtype> transformer(trans_param, caffe::TEST);

	transformer.Transform(lt_img, lt_blob);
	transformer.Transform(rt_img, rt_blob);
	transformer.Transform(lb_img, lb_blob);
	transformer.Transform(rb_img, rb_blob);
	transformer.Transform(ctr_img, ctr_blob);

	_blob_vec.push_back(lt_blob);
	_blob_vec.push_back(rt_blob);
	_blob_vec.push_back(lb_blob);
	_blob_vec.push_back(rb_blob);
	_blob_vec.push_back(ctr_blob);
}

/*
Method : classify

Classify gender of person in the image.

1. Crop image into 5 images and get a blob vector which contains 5 blobs.
2. Get probabilities of each image. Finally, you got 5 probabilities.
3. Get average probabilities of these 5 blobs.
4. Get the bigger probability, male or female.

Param :
1. _img : [in] input image
2. _prob_vec : [out] probabilities of male and female
*/
#if !defined(WIN32) && !defined(__ALTIVEC__)
#define TPL_DTYPE <Dtype>
#else
#define TPL_DTYPE
#endif

int GenderNet::classify(Mat _img, vector<Dtype>& _prob_vec)
{
	vector<Hipe_blob *> input_blob_vec;
	Dtype probability_male = 0;
	Dtype probability_female = 0;

	this->makeBlobVecWithCroppedImg(_img, input_blob_vec);

	for (auto cropped_blob : input_blob_vec)
	{
		this->gender_net->input_blobs()[0]->CopyFrom(*(cropped_blob));
		this->gender_net->Forward();
		Dtype * result = this->gender_net->output_blobs()[0]->mutable_cpu_data TPL_DTYPE ();

		probability_male += result[0];
		probability_female += result[1];
	}

	probability_male /= input_blob_vec.size();
	probability_female /= input_blob_vec.size();

	_prob_vec.push_back(probability_male);
	_prob_vec.push_back(probability_female);

	for (auto cropped_blob : input_blob_vec)
		delete cropped_blob;

	if (probability_male > probability_female)
		return 0;
	else
		return 1;
}
#endif // USE_CAFFE
