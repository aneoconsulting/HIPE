#ifdef __cplusplus
#define __STDINT_MACROS
#define __STDC_CONSTANT_MACROS
#endif
#include <iostream>
#include <concurrent_queue.h>

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