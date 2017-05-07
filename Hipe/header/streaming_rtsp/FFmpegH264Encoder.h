//
//  FFmpegH264Encoder.h
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#ifndef MESAI_FFMPEGH264_ENCODER_H
#define MESAI_FFMPEGH264_ENCODER_H

#include <string>
#include <queue>

#include <functional>

#include <opencv2/opencv.hpp>
#include <atomic>

extern "C" {
	
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <math.h>
	#include <libavutil/opt.h>
	#include <libavutil/mathematics.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libswresample/swresample.h>
	#include <libavutil/imgutils.h>
	#include <libavcodec/avcodec.h>

}

#include <mutex>


namespace MESAI
{
	class FrameStructure {
		public:
			uint8_t * dataPointer;
			int dataSize;
			int frameID;
			~FrameStructure()
			{
				delete dataPointer;
			}
	};

	class FFmpegH264Encoder
	{
	public:
		FFmpegH264Encoder();
		~FFmpegH264Encoder();
		
		void setCallbackFunctionFrameIsReady(std::function<void()> func);
		void convertImageMult2(cv::Mat& image);
		void SetupVideo(std::string filename, int Width, int Height, int FPS, int GOB, int BitPerSecond);
		void CloseVideo();
		void SetupCodec(const char *filename, int codec_id);
		void CloseCodec();
		

		void SendNewFrame(cv::Mat RGBFrame);
		void WriteFrame(cv::Mat & RGBFrame);
		char ReleaseFrame();

		void run();	
		char GetFrame(uint8_t** FrameBuffer, unsigned int *FrameSize);


		std::atomic<bool> quit;
	private:


		std::queue<cv::Mat> inqueue;
		std::mutex inqueue_mutex;
		std::queue<FrameStructure *> outqueue;
		std::mutex outqueue_mutex;


		int m_sws_flags;
		int	m_AVIMOV_FPS;
		int	m_AVIMOV_GOB;
		int	m_AVIMOV_BPS;
		int m_frame_count;
		int	m_AVIMOV_WIDTH;
		int	m_AVIMOV_HEIGHT;
		std::string m_filename;

	    double m_video_time;

	    AVCodecContext *m_c;
		AVStream *m_video_st;
		AVOutputFormat *m_fmt;
		AVFormatContext *m_oc;
		AVCodec *m_video_codec;
		//AVFrame * m_src_picture;
		AVFrame *m_dst_picture;
	    SwsContext *sws_ctx;
	    int bufferSize;

		std::function<void()> onFrame;

	

	};
}
#endif