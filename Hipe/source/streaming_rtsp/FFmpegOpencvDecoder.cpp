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