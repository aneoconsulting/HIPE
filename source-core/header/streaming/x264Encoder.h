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

#ifdef __cplusplus
#define __STDINT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#include <iostream>

#include <queue>
#include <stdint.h>

#include <opencv2/opencv.hpp>


extern "C" {
#include <x264.h>
#include <libswscale/swscale.h>
}

class x264Encoder
{
public:
	x264Encoder(void);
	~x264Encoder(void);

public:
	void initilize();
	void initContext(cv::Size size);

	void unInitilize();
	void convertImageMult2(cv::Mat& image);
	void encodeFrame(cv::Mat& image);
	bool isNalsAvailableInOutputQueue();
	x264_nal_t getNalUnit();
private:
	// Use this context to convert your BGR Image to YUV image since x264 do not support RGB input
	SwsContext* convertContext;
	std::queue<x264_nal_t> outputQueue;
	x264_param_t parameters;
	x264_picture_t picture_in, picture_out;
	x264_t* encoder;
	bool isInit;
};
