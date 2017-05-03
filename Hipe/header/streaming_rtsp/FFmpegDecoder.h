//
//  FFmpegDecoder.h
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#ifndef MESAI_FFmpegDecoder_H
#define MESAI_FFmpegDecoder_H

#include <iostream>
#include <string>

#include <functional>
#include <opencv2/opencv.hpp>


extern "C" {
#include <libavutil/mathematics.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libavutil/pixdesc.h>
#include <libavdevice/avdevice.h>
}



namespace MESAI
{
	class FFmpegDecoder
	{
		public:
            FFmpegDecoder(std::string);
			
			virtual ~FFmpegDecoder();
        
			virtual void intialize();

			virtual void playMedia();

			virtual void finalize();
        
            void setOnframeCallbackFunction(std::function<void(cv::Mat &)> func);
        
            int width;
        
            int height;
        
            int GOP;
        
            int frameRate;
        
            int bitrate;
        
            std::function<void(cv::Mat &)> onFrame;

		protected:
        
            std::string path;
        
			AVCodecContext  *pCodecCtx;

			AVFormatContext *pFormatCtx;
        
            AVFrame *pFrameRGB;
        
            struct SwsContext * img_convert_ctx;

			int videoStream;
        

	};


}

#endif