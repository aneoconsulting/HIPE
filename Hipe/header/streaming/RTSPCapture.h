//@HIPE_LICENSE@
#pragma once
#include <core/HipeStatus.h>
#include <string>


extern "C"
{
	#include <libavutil/frame.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavdevice/avdevice.h>
}
#include <core/HipeException.h>
#include <atomic>
#include <iostream>

#include <opencv2/core/mat.hpp>
#include <data/DirectoryImgData.h>
#include <boost/thread/thread.hpp>
#include <core/queue/ConcurrentQueue.h>
#include "CaptureVideo.h"


class RTSPCapture : public CaptureVideo
{
	AVCodecContext  *pCodecCtx;
	AVFormatContext *pFormatCtx;
	AVCodec *pCodec;
	AVFrame *pFrame;
	AVFrame *pFrameRGB;
	std::atomic<bool> _isInit;
	std::atomic<bool> _isOpened;

	AVPacket *packet;
	int video_stream_index = -1;
	boost::thread task;
	core::queue::ConcurrentQueue<cv::Mat> buffer;

public:

	RTSPCapture(const std::string &path) : CaptureVideo(path)
	{
		_isInit = false;
		_isOpened = false;
		packet = nullptr;
		pCodecCtx = nullptr;
		pCodec = nullptr;
		pFrame = nullptr;
		pFrameRGB = nullptr;
	}

	HipeStatus open()
	{
		if (_isOpened == true)
			return OK;

		if (avformat_open_input(&pFormatCtx, _path.c_str(), NULL, NULL) != 0) {
			std::cout << "Open File Error 12" << std::endl;
			return UNKOWN_ERROR;
		}


		if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
			std::cout << "Get Stream Information Error 13" << std::endl;
			avformat_close_input(&pFormatCtx);
			pFormatCtx = NULL;
			return UNKOWN_ERROR;
		}
		av_dump_format(pFormatCtx, 0, _path.c_str(), 0);
	
		for (int i = 0; i < pFormatCtx->nb_streams; i++) {

			if (pFormatCtx->streams[i]->codec->coder_type == AVMEDIA_TYPE_VIDEO) {
				video_stream_index = i;
				break;
			}
		}

		if (video_stream_index < 0) {
			std::cout << "Video stream was not found Error 14" << std::endl;
			avformat_close_input(&pFormatCtx);
			pFormatCtx = NULL;
			return UNKOWN_ERROR;
		}


		pCodecCtx = pFormatCtx->streams[video_stream_index]->codec;
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		if (pCodec == NULL) {
			std::cout << "codec not found Error 15" << std::endl;
			return UNKOWN_ERROR;
		}

		if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
			std::cout << "Open Codec Error 16" << std::endl;
			return UNKOWN_ERROR;
		}

		AVPixelFormat pFormat = AV_PIX_FMT_RGB24;
		int numBytes = avpicture_get_size(pFormat, pCodecCtx->width, pCodecCtx->height);
		uint8_t *data = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
		avpicture_fill((AVPicture *)pFrameRGB, data, pFormat, pCodecCtx->width, pCodecCtx->height);

		int y_size = pCodecCtx->width * pCodecCtx->height;

		av_new_packet(packet, y_size);

		task = boost::thread(boost::bind(&RTSPCapture::captureTask, this));
		buffer.readyToListen();
		
		_isOpened = true;

		return OK;
	}

	HipeStatus close()
	{
		buffer.stopListening();
		if (task.joinable())
			task.join();
		else
			task.interrupt();

		if (packet != nullptr) av_free_packet(packet);
		avcodec_close(pCodecCtx);
		_isInit = false;
		return OK;
	}

	void captureTask()
	{
		int res;
		int frameFinished;
		while (res = av_read_frame(pFormatCtx, packet) >= 0)
		{
			if (packet->stream_index == video_stream_index) {
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet);

				if (frameFinished) {
					struct SwsContext *img_convert_ctx;
					img_convert_ctx = sws_getCachedContext(nullptr,
						pCodecCtx->width,
						pCodecCtx->height,
						pCodecCtx->pix_fmt,
						pCodecCtx->width,
						pCodecCtx->height,
						AV_PIX_FMT_BGR24,
						SWS_BICUBIC,
						NULL,
						NULL,
						NULL);

					sws_scale(img_convert_ctx,
						((AVPicture*)pFrame)->data,
						((AVPicture*)pFrame)->linesize,
						0,
						pCodecCtx->height,
						((AVPicture *)pFrameRGB)->data,
						((AVPicture *)pFrameRGB)->linesize);

					cv::Mat img(pFrame->height, pFrame->width, CV_8UC3, pFrameRGB->data[0]);

					if (buffer.hasListener())
						buffer.push(img);
					else
					{
						av_free_packet(packet);
						sws_freeContext(img_convert_ctx);
						buffer.stopListening();
						break;
					}

				/*	cv::imshow("Display", img);
					cv::waitKey(30);*/

					av_free_packet(packet);
					sws_freeContext(img_convert_ctx);
				}
			}
		}
		buffer.stopListening();
	}

	

	HipeStatus create()
	{
		if (_isInit == true) // TODO think about multithread capture request
			return OK;

		av_register_all();
		avdevice_register_all();
		avcodec_register_all();
		avformat_network_init();
		pFormatCtx = avformat_alloc_context();
		if (pFormatCtx == nullptr)
		{
			throw HipeException("Cannot allocate ffmpeg formatContext");
			return HipeStatus::UNKOWN_ERROR;
		}

		pFrame = av_frame_alloc();
		pFrameRGB = av_frame_alloc();

		packet = static_cast<AVPacket *>(malloc(sizeof(AVPacket)));

		_isInit = true;

		return OK;
	}

	HipeStatus destroy()
	{
		buffer.stopListening();

		if (pFrame)
		{
			av_free(pFrame);
			pFrame = nullptr;
		}
		if (pFrameRGB)
		{
			av_free(pFrameRGB); 
			pFrameRGB = nullptr;
		}
		if (pFormatCtx)
		{
			avformat_close_input(&pFormatCtx); 
			pFormatCtx = nullptr;
		}

		return OK;
	}

	HipeStatus read(cv::Mat &image)
	{
		if (buffer.hasListener() == false)
			return END_OF_STREAM;

		if (buffer.waituntil_and_pop(image) != true)
			return UNKOWN_ERROR;

		return OK;
	}


public: //getter and setter


	bool isOpened() const
	{
		return _isOpened;
	}

};
