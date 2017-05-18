//
//  FFmpegH264Encoder.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include "FFmpegH264Encoder.h"
#include <opencv2/core/mat.hpp>
#include <filter/data/FileImageData.h>

namespace MESAI
{
	FFmpegH264Encoder::FFmpegH264Encoder() : quit(false)
	{
		

	}

	void FFmpegH264Encoder::setCallbackFunctionFrameIsReady(std::function<void()> func)
	{
		onFrame = func;
	}

#define FFALIGN(x, a) (((x)+(a)-1)&~((a)-1))

	void FFmpegH264Encoder::convertImageMult2(cv::Mat & image)
	{
		int rows = FFALIGN(image.rows, 2);

		int cols = FFALIGN(image.cols, 2);

		if (image.rows != rows || image.cols != cols)
		{
			cv::Mat newImage = cv::Mat::zeros(rows, cols, CV_8UC3);

			image.copyTo(newImage(cv::Rect(0, 0, image.cols, image.rows)));
			image = newImage;
		}
	}

	void FFmpegH264Encoder::SendNewFrame(cv::Mat RGBFrame) {
		inqueue_mutex.lock();
		if(inqueue.size()<30)
		{
			convertImageMult2(RGBFrame);
			inqueue.push(RGBFrame);
		}
		inqueue_mutex.unlock();
	}

	void FFmpegH264Encoder::run()
	{
		while(!quit)
		{
			if(!inqueue.empty())
			{
				cv::Mat frame;
				inqueue_mutex.lock();
				frame = inqueue.front();
				inqueue.pop();
				inqueue_mutex.unlock();
				if(!frame.empty())
				{
					WriteFrame(frame);
				}
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::microseconds(300));
			}
        }
	}

	void FFmpegH264Encoder::SetupCodec(const char *filename, int codec_id)
	{
		int ret;
		m_sws_flags = SWS_FAST_BILINEAR;
		m_frame_count=0;
		
		avcodec_register_all();
		av_register_all();
		
		avformat_alloc_output_context2(&m_oc, nullptr, nullptr, filename);
		
		if (!m_oc) {
			avformat_alloc_output_context2(&m_oc, nullptr, "avi", filename);
		}

		if (!m_oc) {
			return;
		}

		m_fmt = m_oc->oformat;
		m_video_st = nullptr;
		m_fmt->video_codec = (AVCodecID)codec_id;
		m_fmt->audio_codec = AV_CODEC_ID_NONE;

		AVStream *st;

		m_video_codec = avcodec_find_encoder(m_fmt->video_codec);
		if (!(m_video_codec)) {
				return;
		}

		st = avformat_new_stream(m_oc, m_video_codec);

		if (!st) {
				return;
		}

		st->id = m_oc->nb_streams-1;

		m_c = st->codec;
		
		m_c->codec_id = m_fmt->video_codec;
		m_c->bit_rate = 640 * 480 * 4 * 8 * m_AVIMOV_FPS;			//Bits Per Second 
		m_c->width    = 640;			//Note Resolution must be a multiple of 2!!
		m_c->height   = 480;		//Note Resolution must be a multiple of 2!!
		m_c->time_base.den = m_AVIMOV_FPS;		//Frames per second
		m_c->time_base.num = 1;
		m_c->gop_size      = m_AVIMOV_GOB;		// Intra frames per x P frames
		m_c->pix_fmt       = AV_PIX_FMT_YUV420P;//Do not change this, H264 needs YUV format not RGB
	

		if (m_oc->oformat->flags & AVFMT_GLOBALHEADER)
			m_c->flags |= CODEC_FLAG_GLOBAL_HEADER;

		m_video_st=st;
		

		AVCodecContext *c = m_video_st->codec;

		ret = avcodec_open2(c, m_video_codec, nullptr);
		if (ret < 0) {
			return;
		}

		//ret = avpicture_alloc(&m_dst_picture, c->pix_fmt, c->width, c->height);
		m_dst_picture = av_frame_alloc();
		m_dst_picture->format = c->pix_fmt;
		m_dst_picture->data[0] = nullptr;
        m_dst_picture->linesize[0] = -1;
		m_dst_picture->pts = 0;
        m_dst_picture->width = m_c->width;
        m_dst_picture->height = m_c->height;

		ret = av_image_alloc(m_dst_picture->data, m_dst_picture->linesize, m_dst_picture->width, m_dst_picture->height, (AVPixelFormat)m_dst_picture->format, 32);
		if (ret < 0) {
			return;
		}

		//ret = avpicture_alloc(&m_src_picture, AV_PIX_FMT_BGR24, c->width, c->height);
		/*m_src_picture = av_frame_alloc();
		m_src_picture->format = c->pix_fmt;
		ret = av_image_alloc(m_src_picture->data, m_src_picture->linesize, c->width, c->height, AV_PIX_FMT_BGR24, 24);

		if (ret < 0) {
			return;
		}*/

		bufferSize = ret;
		
		av_dump_format(m_oc, 0, filename, 1);

		if (!(m_fmt->flags & AVFMT_NOFILE)) {
			ret = avio_open(&m_oc->pb, filename, AVIO_FLAG_WRITE);
			if (ret < 0) {
				return;
			}
		}
		
		ret = avformat_write_header(m_oc, nullptr);
		
		if (ret < 0) {
			return;
		}

		sws_ctx = sws_getContext(m_AVIMOV_WIDTH, m_AVIMOV_HEIGHT, AV_PIX_FMT_BGR24,
								 c->width, c->height, AV_PIX_FMT_YUV420P,
								 SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
		if (!sws_ctx) {
			return;
		}
	}

	void fill_data(uint8_t* data, int buffer_size, int linesize, const cv::Mat& mat)
	{
		int endbuffer = linesize - mat.cols * 3;
		for (int i = 0; i < mat.rows; i++)
		{
			const uint8_t *data_src = mat.ptr<uint8_t>(i);
			memcpy(&(data[i * linesize]), data_src, mat.cols * 3);
			
			
			memset(&(data[i * linesize + mat.cols * 3]), 0, endbuffer);
		}
	}


	void FFmpegH264Encoder::WriteFrame(cv::Mat & RGBFrame )
	{	
		//fill_data(m_src_picture->data[0], bufferSize, m_src_picture->linesize[0], RGBFrame);

		//memcpy(m_src_picture->data[0], RGBFrame.data, bufferSize);
		int stride = RGBFrame.cols * 3;

		sws_scale(sws_ctx,
			&(RGBFrame.data), &stride,
					0, m_AVIMOV_HEIGHT, m_dst_picture->data, m_dst_picture->linesize);
		

        AVPacket pkt = { 0 };
		int got_packet;
		av_init_packet(&pkt);

		int ret = 0;
		timeval current_time, after;
		//hipe_gettimeofday(&current_time, nullptr);
		ret = avcodec_encode_video2(m_c, &pkt, m_dst_picture, &got_packet);
		//hipe_gettimeofday(&after, nullptr);
		//double elapse = ((after.tv_sec - current_time.tv_sec) * 1000000L + after.tv_usec) - current_time.tv_usec;
		//elapse /= 1000.;

		//std::cout << "Time spent : " << elapse << " ms" << std::endl;
		if (ret < 0) {
			return;
		}

		if (!ret && got_packet && pkt.size) 
		{
			pkt.stream_index = m_video_st->index;
			FrameStructure * frame = new FrameStructure();
			frame->dataPointer = new uint8_t[pkt.size];
			frame->dataSize = pkt.size-4;
			frame->frameID = m_frame_count;

			memcpy(frame->dataPointer,pkt.data+4,pkt.size-4);

			outqueue_mutex.lock();

			if(outqueue.size()<30)
			{
				outqueue.push(frame);
			}
			else
			{
				delete frame;
			}

			outqueue_mutex.unlock();

		}

		av_free_packet(&pkt);

		m_frame_count++;
		m_dst_picture->pts += av_rescale_q(1, m_video_st->codec->time_base, m_video_st->time_base);
		
		onFrame();
	}

	void FFmpegH264Encoder::SetupVideo(std::string filename, int Width, int Height, int FPS, int GOB, int BitPerSecond)
	{
		m_filename = filename;
		m_AVIMOV_WIDTH=Width;	//Movie width
		m_AVIMOV_HEIGHT=Height;	//Movie height
		m_AVIMOV_FPS=FPS;		//Movie frames per second
		m_AVIMOV_GOB=GOB;		//I frames per no of P frames, see note below!
		m_AVIMOV_BPS=BitPerSecond; //Bits per second, if this is too low then movie will become garbled
		
		SetupCodec(m_filename.c_str(),AV_CODEC_ID_H264);	
	}

	void FFmpegH264Encoder::CloseCodec()
	{

		av_write_trailer(m_oc);
	    avcodec_close(m_video_st->codec);

	    av_freep(&(m_dst_picture->data[0]));
        av_frame_unref(m_dst_picture);
        av_free(m_dst_picture);
       /* av_freep(&(m_src_picture->data[0]));
        av_frame_unref(m_src_picture);
        av_free(m_src_picture);*/

	    if (!(m_fmt->flags & AVFMT_NOFILE))
	        avio_close(m_oc->pb);

        m_oc->pb = nullptr;

	    avformat_free_context(m_oc);
	    sws_freeContext(sws_ctx);

	}

	void FFmpegH264Encoder::CloseVideo()
	{
		CloseCodec();	
	}

	char FFmpegH264Encoder::GetFrame(uint8_t** FrameBuffer, unsigned int *FrameSize)
	{	
		if(!outqueue.empty())
		{	
			FrameStructure * frame;
			frame  = outqueue.front();
			*FrameBuffer = (uint8_t*)frame->dataPointer;
			*FrameSize = frame->dataSize;
			return 1;
		}
		else
		{
			*FrameBuffer = 0;
			*FrameSize = 0;
			return 0;
		}
	}

	char FFmpegH264Encoder::ReleaseFrame()
	{
		outqueue_mutex.lock();
		if(!outqueue.empty())
		{
			FrameStructure * frame = outqueue.front();
			outqueue.pop();	
			delete frame;
		}
		outqueue_mutex.unlock();
		return 1;
	}
}
