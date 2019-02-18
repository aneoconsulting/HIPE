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
#include "Caffe_os_deps.h"

#ifdef USE_CAFFE
#define GLOG_NO_ABBREVIATED_SEVERITIES
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN 
#define NO_STRICT
#ifdef STRICT
#undef STRICT
#endif

#include <opencv2/dnn/dnn.hpp>
#include <caffe/caffe.hpp>
#include <caffe/data_transformer.hpp>

#include <string>
#include <vector>

#define	AGE_GROUP_NUM	8

using namespace std;
using namespace cv;
using namespace caffe;

typedef double Dtype;

/*
Class : AgeNet

This class is resopnsible for classifying age with given face image.
*/

class AgeNet
{
private:
	string	model_file;			// model file path (deep_age2.prototxt)
	string	weight_file;			// weight file path (age_net.caffemodel)
	string	mean_file;			// mean file path (mean.binaryproto)

	std::shared_ptr<Hipe_net> age_net;		// Deep Convolution Network

public:

	// Constructor
	AgeNet(const string _model_file, const string _weight_file, const string _mean_file)
	{
		model_file.assign(_model_file);
		weight_file.assign(_weight_file);
		mean_file.assign(_mean_file);
	}

	// Initialize age_net
	void initNetwork();

	// Get cv::Mat from mean_file
	void getMeanImgFromMeanFile(Mat& _mean_img);

	// Get blob vector which contains 5 input blobs (Details in implementation)
	void makeBlobVecWithCroppedImg(Mat _img, vector<Hipe_blob *>& _blob_vec);

	// Classify age and get probability
	int classify(Mat _img, vector<Dtype>& prob_vec);
};
#endif //USE_CAFFE
