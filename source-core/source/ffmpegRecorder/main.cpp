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


#include <iostream>
#include <opencv2/highgui/highgui.hpp>


extern "C" {
#include <libavutil/opt.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}


int main(int argc, char* argv[])
{
	av_register_all();
	avdevice_register_all();
	avcodec_register_all();
	avformat_network_init();

	const char  *filenameSrc = "rtsp://192.168.1.12:8086"; //Axis

	AVCodecContext  *pCodecCtx;
	AVFormatContext *pFormatCtx;
	AVCodec *pCodec;
	AVFrame *pFrame;
	AVFrame *pFrameRGB;

	pFormatCtx = avformat_alloc_context();
	if (pFormatCtx == NULL)
		return -8;


	if (avformat_open_input(&pFormatCtx, filenameSrc, NULL, NULL) != 0) {
		std::cout << "Open File Error 12" << std::endl;
		return -12;
	}


	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		std::cout << "Get Stream Information Error 13" << std::endl;
		avformat_close_input(&pFormatCtx);
		pFormatCtx = NULL;
		return -13;
	}
	av_dump_format(pFormatCtx, 0, filenameSrc, 0);
	int video_stream_index = -1;


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
		return -14;
	}


	pCodecCtx = pFormatCtx->streams[video_stream_index]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) {
		std::cout << "codec not found Error 15" << std::endl;
		return -15;
	}

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		std::cout << "Open Codec Error 16" << std::endl;
		return -16;
	}


	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();

	AVPixelFormat pFormat = AV_PIX_FMT_RGB24;
	int numBytes = avpicture_get_size(pFormat, pCodecCtx->width, pCodecCtx->height);
	uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	avpicture_fill((AVPicture *)pFrameRGB, buffer, pFormat, pCodecCtx->width, pCodecCtx->height);

	int y_size = pCodecCtx->width * pCodecCtx->height;
	AVPacket *packet = (AVPacket *)malloc(sizeof(AVPacket));
	av_new_packet(packet, y_size);

	int res;
	int frameFinished;
	while (res = av_read_frame(pFormatCtx, packet) >= 0)
	{
		if (packet->stream_index == video_stream_index) {
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet);

			if (frameFinished) {
				struct SwsContext *img_convert_ctx;
				img_convert_ctx = sws_getCachedContext(NULL,
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
				cv::imshow("Display", img);
				cv::waitKey(30);

				av_free_packet(packet);
				sws_freeContext(img_convert_ctx);
			}
		}
	}

	av_free_packet(packet);
	avcodec_close(pCodecCtx);
	av_free(pFrame);
	av_free(pFrameRGB);
	avformat_close_input(&pFormatCtx);

	return (EXIT_SUCCESS);
}
