#pragma once
#include <queue>
#include <x264Encoder.h>
#include <opencv2/opencv.hpp>
#include <FramedSource.hh>
#include <core/queue/ConcurrentQueue.h>
 
#include <filter/data/IOData.h>

class LiveSourceWithx264 :public FramedSource
{
public:
	static LiveSourceWithx264* createNew(UsageEnvironment& env, core::queue::ConcurrentQueue<filter::data::IOData> & concurrent_queue);
	static EventTriggerId eventTriggerId;
protected:
	LiveSourceWithx264(UsageEnvironment& env, core::queue::ConcurrentQueue<filter::data::IOData> & concurrent_queue);
	virtual ~LiveSourceWithx264(void);
private:
	virtual void doGetNextFrame();
	static void deliverFrame0(void* clientData);
	void deliverFrame();
	void encodeNewFrame();
	static unsigned referenceCount;
	std::queue<x264_nal_t> nalQueue;
	timeval currentTime;
	// videoCaptureDevice is my BGR data source. You can have according to your need
	//cv::VideoCapture videoCaptureDevice;
	core::queue::ConcurrentQueue<filter::data::IOData> & _concurrent_queue;

	cv::Mat rawImage;
	// Remember the x264 encoder wrapper we wrote in the start
	x264Encoder *encoder;
	unsigned int fPlayTimePerFrame;
	unsigned int fPreferredFrameSize;
	unsigned int fLastPlayTime;
};
