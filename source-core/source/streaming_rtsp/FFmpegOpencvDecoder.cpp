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

#include <FFmpegOpencvDecoder.h>
#include <opencv2/opencv.hpp>
#include <core/misc.h>


void FFmpegOpencvDecoder::intialize()
{
	

	height = _conf.height;
	width = _conf.width;
	bitrate = _conf.bit_rate;
	GOP = _conf.gop_size;
	frameRate = _conf.fps;

	

	video.open(0);
	cv::Mat frame;
	int count_frame = 0;
	video >> frame;
	height = frame.rows;
	width = frame.cols;
	
	//compute frameRate
	struct timeval current;
	struct timeval next;
	hipe_gettimeofday(&current, nullptr);
	next.tv_sec = next.tv_usec = 0;
	unsigned long long elapse = 0;
	while (elapse < 1000000) // 100 ms;
	{
		video >> frame;
		hipe_gettimeofday(&next, nullptr);
		elapse = ((next.tv_sec - current.tv_sec) * 1000000L + next.tv_usec) - current.tv_usec;
		count_frame++;
	}
	double fps = (count_frame * 1000000L) / elapse;
	frameRate = fps;
	GOP = 0;
	bitrate = height * width * frameRate;
}

void FFmpegOpencvDecoder::playMedia()
{
	while (video.isOpened())
	{
		cv::Mat frame;
		video >> frame;
		onFrame(frame);
	}
	
}
