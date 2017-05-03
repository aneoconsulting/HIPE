///
//
// LibSourcey
// Copyright (c) 2005, Sourcey <http://sourcey.com>
//
// SPDX-License-Identifier:	LGPL-2.1+
//
///


#include "scy/idler.h"
#include "scy/logger.h"
#include "scy/filesystem.h"
#include "signaler.h"

#include "webrtc/base/ssladapter.h"
#include "webrtc/base/thread.h"
#include <scy/symple/client.h>
#include <scy/application.h>
#include <core/HipeException.h>


using std::endl;
using namespace scy;


// Sample data directory helper
std::string sampleDataDir(const std::string& file)
{
    std::string dir;
    fs::addnode(dir, SCY_SOURCE_DIR);
    fs::addnode(dir, "av");
    fs::addnode(dir, "samples");
    fs::addnode(dir, "data");
    if (!file.empty())
        fs::addnode(dir, file);
    return dir;
}

int main2(int argc, char** argv);

int main(int argc, char** argv)
{
    Logger::instance().add(new ConsoleChannel("debug", LInfo)); // LTrace, LDebug
    Logger::instance().setWriter(new AsyncLogWriter);
	main2(argc, argv);
//#if USE_SSL
    // net::SSLManager::initNoVerifyClient();
//#endif

    // Pre-initialize video captures in the main thread
    // av::MediaFactory::instance().loadVideoCaptures();

    // Setup WebRTC environment
    rtc::LogMessage::LogToDebug(rtc::LS_INFO); // LS_VERBOSE, LS_INFO, LERROR
    rtc::LogMessage::LogTimestamps();
    rtc::LogMessage::LogThreads();

    rtc::InitializeSSL();

    {
        // Video source file
        //std::string sourceFile(sampleDataDir("test.mp4"));
		//std::string sourceFile("C:\\workspace\\ANEO\\Project\\Poclain-Hydraulics\\Data\\Images\\OK.JPG");
		std::string sourceFile("C:\\workspace\\ANEO\\Project\\Poclain\\Data\\Images\\Essai_du_08Juin2016\\vue_1.mp4");
	
        // Symple signalling client options
        smpl::Client::Options options;
        options.host = SERVER_HOST;
        options.port = SERVER_PORT;
        options.name = "Video Server";
        options.user = "videoserver";

        // NOTE: The server must enable anonymous
        // authentication for this test.
        // options.token = ""; token based authentication

        // Parse command line args
        OptionParser optparse(argc, argv, "-");
        for (auto& kv : optparse.args) {
            const std::string& key = kv.first;
            const std::string& value = kv.second;
            DebugA("Setting option: ", key, ": ", value)
            if (key == "file") {
                sourceFile = value;
            }
            else {
                std::cerr << "Unrecognized command: " << key << "=" << value << endl;
            }
        }

        Signaler app(options);
        app.startStreaming(sourceFile, true);

        // Process WebRTC threads on the main loop.
        Idler rtc(app.loop, [](void* arg) {
            auto thread = reinterpret_cast<rtc::Thread*>(arg);
            thread->ProcessMessages(10);
        }, rtc::Thread::Current());

		core::queue::ConcurrentQueue<cv::Mat> & queue_mat = app.getQueueMat();

		/*cv::Mat ok = cv::imread(sourceFile);
		cv::resize(ok, ok, cv::Size(ok.cols / 5, ok.rows / 5));
		cv::imshow("test", ok);
		cv::waitKey();*/
		cv::VideoCapture _capture;
		std::thread t([&queue_mat, &_capture, &sourceFile]() {
			std::cout << "thread function\n";
			if (!_capture.isOpened())
			{
				_capture.open(sourceFile);

				if (!_capture.isOpened())
				{
					std::stringstream str;
					str << "Cannot open video : " << sourceFile;
					throw HipeException(str.str());
				}
			}

			bool OK = _capture.grab();
			if (!OK)
			{
				return false;
			}
		
			cv::Mat frame;

			_capture.read(frame);

			while (frame.rows > 0 && frame.cols > 0)
			{

				if (!_capture.isOpened() || !_capture.grab())
				{
					return false;
				}
				_capture.read(frame);
				if (frame.rows > 0 && frame.cols > 0)
					queue_mat.push(frame);
			}


		});
		app.waitForShutdown();

       
        // app.finalize();
    }

    // Shutdown the media factory and release devices
    // av::MediaFactory::instance().unloadVideoCaptures();
    // av::MediaFactory::shutdown();

//#if USE_SSL
    // net::SSLManager::destroy();
//#endif
    rtc::CleanupSSL();
    Logger::destroy();
    return 0;
}

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "../../../../HipeExternal/win64/ffmpeg/include/libavutil/pixfmt.h"
#include "../../../../HipeExternal/win64/ffmpeg/include/libavutil/avutil.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
}


int main2(int argc, char* argv[])
{
	av_register_all();
	avdevice_register_all();
	avcodec_register_all();
	avformat_network_init();

	const char  *filenameSrc = "rtsp://admin:pass@192.168.10.211/axis-media/media.amp"; //Axis

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


	if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
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
