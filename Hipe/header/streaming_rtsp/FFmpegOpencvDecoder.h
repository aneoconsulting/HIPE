//@HIPE_LICENSE@
#pragma once
#include <FFmpegDecoder.h>
#include <opencv2/videoio.hpp>

class FFmpegOpencvDecoder : public MESAI::FFmpegDecoder
{
public:
	struct config
	{
		int height;
		int width;
		int bit_rate;
		int gop_size;
		int fps;
	};
	FFmpegOpencvDecoder::config _conf;
	cv::VideoCapture video;

	FFmpegOpencvDecoder(FFmpegOpencvDecoder::config conf) : FFmpegDecoder("OpencvCapture"), _conf(conf)
	{
		
	}

	virtual void intialize();

	virtual void playMedia();
};
