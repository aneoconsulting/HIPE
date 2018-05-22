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

#pragma once

#include <iostream>
#include <vector>
#include <scy/av/mediacapture.h>
#include <core/queue/ConcurrentQueue.h>
#include <libavutil/opt.h>
#include "OCVVideoDecoder.h"

// FFmpeg
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
}
// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

class OCVVideoCapture : public scy::av::MediaCapture
{
	AVCodec *encoder;
	AVFormatContext* outContainer;
	AVStream *outStream;
	core::queue::ConcurrentQueue<cv::Mat> _queueMat;
	friend class scy::av::MediaCapture;

public:
	core::queue::ConcurrentQueue<cv::Mat> & getQueueMat() 
	{
		return _queueMat;
	}


	OCVVideoCapture()
	{
		//avcodec_register_all();
		encoder = avcodec_find_decoder(AV_CODEC_ID_H264);
		outContainer = avformat_alloc_context();
		AVCodecContext* av_codec_context = avcodec_alloc_context3(encoder);
		av_codec_context->codec_id = AV_CODEC_ID_H264;
		av_codec_context->codec_type = AVMEDIA_TYPE_VIDEO;
		av_codec_context->gop_size = 30;
		/*av_codec_context->bit_rate = 1024 * 768 * 4;
		av_codec_context->width = 1024;
		av_codec_context->height = 768;*/
		AVRational values = { 1, 29 };
		av_codec_context->time_base = values;
		av_codec_context->max_b_frames = 1;
		av_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
		av_codec_context->flags = CODEC_FLAG_GLOBAL_HEADER;
		AVDictionary * opts(0);
		/*av_dict_set(&opts, "b", "2.5M", 0);
		av_opt_set(av_codec_context->priv_data, "preset", "veryfast", 0);*/
		outStream = avformat_new_stream(outContainer, encoder);
		//avcodec_get_context_defaults3(outStream->codec, encoder);
		outStream->codec = av_codec_context;
		outStream->codec->pix_fmt = AV_PIX_FMT_YUV420P;

		const AVRational dst_fps = { 29, 1 };
		outStream->codec->time_base = outStream->time_base = av_inv_q(dst_fps);
		outStream->r_frame_rate = outStream->avg_frame_rate = dst_fps;
		//avformat_write_header(outContainer, nullptr);
		//No way i'll develop this
		_audio = nullptr;
		frame = nullptr;

		if (!_video) {
			scy::av::VideoDecoder* ocv_video_decoder = new scy::av::VideoDecoder(outStream);
			ocv_video_decoder->emitter.attach(scy::packetSlot(this, &OCVVideoCapture::emit));
			//ocv_video_decoder->create(opts);
			ocv_video_decoder->create();
			ocv_video_decoder->open();

			_video = ocv_video_decoder;
			_video->iparams.fps = values.den;
		}

		//TODO ??
		_looping = false;
	}

public:
	virtual void start()
	{
		//TraceS(this) << "Starting" << endl;

		std::lock_guard<std::mutex> guard(_mutex);
		assert(_video);

		if ((_video) && !_thread.running()) {
			//TraceS(this) << "Initializing thread" << endl;
			_stopping = false;
			_thread.start(std::bind(&OCVVideoCapture::run, this));
		}
	}

protected:
	cv::Mat avframe_to_cvmat(AVFrame *frame)
	{
		AVFrame dst;
		cv::Mat m;

		memset(&dst, 0, sizeof(dst));

		int w = frame->width, h = frame->height;
		m = cv::Mat(h, w, CV_8UC3);
		dst.data[0] = (uint8_t *)m.data;
		avpicture_fill((AVPicture *)&dst, dst.data[0], AV_PIX_FMT_BGR24, w, h);

		struct SwsContext *convert_ctx = NULL;
		enum AVPixelFormat src_pixfmt = (enum AVPixelFormat)frame->format;
		enum AVPixelFormat dst_pixfmt = AV_PIX_FMT_BGR24;
		convert_ctx = sws_getContext(w, h, src_pixfmt, w, h, dst_pixfmt,
			SWS_FAST_BILINEAR, NULL, NULL, NULL);
		sws_scale(convert_ctx, frame->data, frame->linesize, 0, h,
			dst.data, dst.linesize);
		sws_freeContext(convert_ctx);

		return m;
	}

	AVFrame cvmat_to_avframe(cv::Mat frame)
	{
		AVFrame dst;
		cv::Size frameSize = frame.size();
		outStream->codec->width = frame.cols;
		outStream->codec->height = frame.rows;

		avpicture_fill((AVPicture*)&dst, frame.data, AV_PIX_FMT_BGR24, outStream->codec->width, outStream->codec->height);
		dst.width = frameSize.width;
		dst.height = frameSize.height;
		
		return dst;
	}

	AVFrame * frame;
	AVFrame * ffmpeg_encoder_set_frame_yuv_from_rgb(cv::Mat & cvFrame)
	{
		outStream->codec->width = cvFrame.cols;
		outStream->codec->height = cvFrame.rows;
		if (frame == nullptr)
		{
			frame = av_frame_alloc();
			frame->format = outStream->codec->pix_fmt;
			frame->width = outStream->codec->width;
			frame->height = outStream->codec->height;
			//frame->key_frame = 1;
			
			int ret = av_image_alloc(frame->data, frame->linesize, outStream->codec->width, outStream->codec->height, outStream->codec->pix_fmt, 32);
			if (ret < 0) {
				fprintf(stderr, "Could not allocate raw picture buffer\n");
				throw std::runtime_error("Fail to allocate Image for AVFrame");
			}
		}
		if (!frame) {
			fprintf(stderr, "Could not allocate video frame\n");
			throw std::runtime_error("Fail to allocate AVFrame");
		}
		
		
		const int in_linesize[1] = { 3 * cvFrame.cols };
		struct SwsContext *convert_ctx = NULL;
		
		convert_ctx = sws_getCachedContext(convert_ctx,
			cvFrame.cols, cvFrame.rows, AV_PIX_FMT_BGR24,
			cvFrame.cols, cvFrame.rows, AV_PIX_FMT_YUV420P,
			0, 0, 0, 0);
		sws_scale(convert_ctx, (const uint8_t * const *)&cvFrame.data, in_linesize, 0,
			cvFrame.rows, frame->data, frame->linesize);
		//avpicture_fill(reinterpret_cast<AVPicture*>(frame), framebuf.data(), vstrm->codec->pix_fmt, dst_width, dst_height);
		return frame;
	}

	virtual void emit(scy::IPacket& packet)
	{
		//TraceS(this) << "Emit: " << packet.size() << endl;

		emitter.emit(packet);
	}

	inline void emitPacket(scy::av::VideoDecoder* dec, AVFrame* frame)
	{
		frame->pts = av_frame_get_best_effort_timestamp(frame);
		
		// Set the decoder time in microseconds
		// This value represents the number of microseconds 
		// that have elapsed since the brginning of the stream.

		dec->time = frame->pts > 0 ? static_cast<int64_t>(frame->pkt_pts *
			av_q2d(dec->stream->time_base) * AV_TIME_BASE) : 0;

		// Set the decoder pts in stream time base
		dec->pts = frame->pts;

		// Set the decoder seconds since stream start
		dec->seconds = (frame->pkt_dts - dec->stream->start_time) * av_q2d(dec->stream->time_base);

		/*TraceL << "Decoded video frame:"
			<< "\n\tFrame DTS: " << frame->pkt_dts
			<< "\n\tFrame PTS: " << frame->pts
			<< "\n\tTimestamp: " << dec->time
			<< "\n\tSeconds: " << dec->seconds
			<< endl;*/

		scy::av::PlanarVideoPacket video(frame->data, frame->linesize, dec->oparams.pixelFmt,
			frame->width, frame->height, dec->time);
		video.source = frame;
		video.opaque = dec;

		dec->emitter.emit(video);
	}


	void run()
	{
		try {
			int res;
			//AVPacket ipacket;
			//av_init_packet(&ipacket);

			// Looping variables
			int64_t videoPtsOffset = 0;
			int64_t audioPtsOffset = 0;

			// Realtime variables
			int64_t lastTimestamp = scy::time::hrtime();
			int64_t frameInterval = _video ? scy::av::fpsToInterval(_video->iparams.fps) : 0;

			// Reset the stream back to the beginning when looping is enabled
			if (_looping) {
				for (unsigned i = 0; i < _formatCtx->nb_streams; i++) {
					if (avformat_seek_file(_formatCtx, i, 0, 0, 0, AVSEEK_FLAG_FRAME) < 0) {
						throw std::runtime_error("Cannot reset media stream");
					}
				}
			}

			_queueMat.readyToListen();
			cv::Mat cvFrame;
			// Read input packets until complete
			while (_queueMat.waituntil_and_pop(cvFrame)) {
				/*TraceS(this) << "Read frame: pts=" << ipacket.pts
					<< ", dts=" << ipacket.dts << endl;*/
				if (_stopping) 
					break;
				if (!_queueMat.hasListener())
				{
					continue;
				}
				
				//Convert this memory grabber function
				AVFrame *avFrame = ffmpeg_encoder_set_frame_yuv_from_rgb(cvFrame);
				frame->format = static_cast<int>(_video->stream->codec->pix_fmt);
				if (_video) {

					// Set the PTS offset when looping
					if (_looping) {
						
						if (avFrame->pts == 0 && _video->pts > 0)
							videoPtsOffset = _video->pts;
						avFrame->pts += videoPtsOffset;
					}

					// Decode and emit
					emitPacket(_video, avFrame);
					{
						/*TraceS(this) << "Decoded video: "
							<< "time=" << _video->time << ", "
							<< "pts=" << _video->pts << endl;*/
					}

					// Pause the input stream in realtime mode if the
					// decoder is working too fast
					//if (_realtime) 
					{
						while ((scy::time::hrtime() - lastTimestamp) < frameInterval) {
							scy::sleep(1);
						}
						lastTimestamp = scy::time::hrtime();
					}
				}
				//else if (_audio && ipacket.stream_index == _audio->stream->index) {

				//	// Set the PTS offset when looping
				//	if (_looping) {
				//		if (ipacket.pts == 0 && _audio->pts > 0)
				//			videoPtsOffset = _audio->pts;
				//		ipacket.pts += audioPtsOffset;
				//	}

				//	// Decode and emit
				//	if (_audio->decode(ipacket)) {
				//	/*	TraceS(this) << "Decoded Audio: "
				//			<< "time=" << _audio->time << ", "
				//			<< "pts=" << _video->pts << endl;*/
				//	}
				//}

				//av_packet_unref(&ipacket);
			}

			// Flush remaining packets
			if (!_stopping && res < 0) {
				if (_video)
					_video->flush();
				/*if (_audio)
					_audio->flush();*/
			}

			// End of file or error
			/*TraceS(this) << "Decoding: EOF: " << res << endl;*/
		}
		catch (std::exception& exc) {
			_error = exc.what();
			/*ErrorS(this) << "Decoder Error: " << _error << endl;*/
		}
		catch (...) {
			_error = "Unknown Error";
			/*ErrorS(this) << "Unknown Error" << endl;*/
		}

		if (_stopping || !_looping) {
			/*TraceS(this) << "Exiting" << endl;*/
			_stopping = true;
			_queueMat.stopListening();
			Closing.emit();
		}
	}
};
