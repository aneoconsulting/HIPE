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

#include "x264Encoder.h"


x264Encoder::x264Encoder(void) : isInit(false)
{
}


x264Encoder::~x264Encoder(void)
{
}

void x264Encoder::initilize()
{
}

void x264Encoder::initContext(cv::Size size)
{
	if (isInit == true && parameters.i_height == size.height && parameters.i_width == size.width) return;

	x264_param_default_preset(&parameters, "veryfast", "zerolatency");
	parameters.i_log_level = X264_LOG_INFO;
	parameters.i_threads = 1;
	parameters.i_width = size.width;
	parameters.i_height = size.height;
	parameters.i_fps_num = 25;
	parameters.i_fps_den = 1;
	parameters.i_keyint_max = 25;
	parameters.b_intra_refresh = 1;
	parameters.rc.i_rc_method = X264_RC_CRF;
	parameters.rc.i_vbv_buffer_size = 1000000;
	parameters.rc.i_vbv_max_bitrate = 90000;
	parameters.rc.f_rf_constant = 25;
	parameters.rc.f_rf_constant_max = 35;
	parameters.i_sps_id = 7;
	// the following two value you should keep 1
	parameters.b_repeat_headers = 1; // to get header before every I-Frame
	parameters.b_annexb = 1; // put start code in front of nal. we will remove start code later
	x264_param_apply_profile(&parameters, "baseline");

	encoder = x264_encoder_open(&parameters);
	x264_picture_alloc(&picture_in, X264_CSP_I420, parameters.i_width, parameters.i_height);
	picture_in.i_type = X264_TYPE_AUTO;
	picture_in.img.i_csp = X264_CSP_I420;
	// i have initilized my color space converter for BGR24 to YUV420 because my opencv video capture gives BGR24 image. You can initilize according to your input pixelFormat
	convertContext = sws_getContext(parameters.i_width, parameters.i_height, AV_PIX_FMT_BGR24, parameters.i_width, parameters.i_height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);
	isInit = true;
}

void x264Encoder::unInitilize()
{
	x264_encoder_close(encoder);
	sws_freeContext(convertContext);
}
void x264Encoder::convertImageMult2(cv::Mat & image)
{
	int rows = (image.rows >> 1) * 2;
	int cols = (image.cols >> 1) * 2;

	if (image.rows != rows || image.cols != cols)
	{
		cv::Mat newImage = cv::Mat::zeros(rows + 2, cols + 2, CV_8UC3);
		
		image.copyTo(newImage(cv::Rect(0, 0, image.cols, image.rows)));
		image = newImage;
	}
}

void x264Encoder::encodeFrame(cv::Mat& image)
{
	convertImageMult2(image);

	initContext(image.size());

	int srcStride = parameters.i_width * 3;
	//int srcStride = image.cols * 3;
	cv::Mat resized = image;
	//cv::resize(image, resized, cv::Size(parameters.i_width, parameters.i_height));

	sws_scale(convertContext, &(resized.data), &srcStride, 0, parameters.i_height, picture_in.img.plane, picture_in.img.i_stride);
	x264_nal_t* nals;
	int i_nals = 0;
	int frameSize = -1;

	frameSize = x264_encoder_encode(encoder, &nals, &i_nals, &picture_in, &picture_out);
	if (frameSize > 0)
	{
		for (int i = 0; i < i_nals; i++)
		{
			outputQueue.push(nals[i]);
		}
	}
}

bool x264Encoder::isNalsAvailableInOutputQueue()
{
	if (outputQueue.empty() == true)
	{
		return false;
	}
	else
	{
		return true;
	}
}

x264_nal_t x264Encoder::getNalUnit()
{
	x264_nal_t nal;
	nal = outputQueue.front();
	outputQueue.pop();
	return nal;
}
