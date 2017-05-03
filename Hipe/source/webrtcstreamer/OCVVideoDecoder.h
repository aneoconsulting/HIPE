#pragma once




#include "scy/av/packet.h"
#include "scy/av/videocontext.h"
#include <scy/av/videodecoder.h>


struct OCVVideoDecoder : public scy::av::VideoDecoder
{
	OCVVideoDecoder(AVStream* stream) : VideoDecoder(stream)
	{
	}

	virtual ~OCVVideoDecoder()
	{
	}
	

	virtual void create(AVDictionary * opts)
	{
		assert(stream);
		//TraceS(this) << "Create: " << stream->index << std::endl;

		ctx = stream->codec;

		codec = avcodec_find_decoder(ctx->codec_id);
		if (!codec)
			throw std::runtime_error("Video codec missing or unsupported.");

		frame = av_frame_alloc();
		if (frame == nullptr)
			throw std::runtime_error("Cannot allocate video input frame.");

		if (avcodec_is_open(ctx) == 0) {
			int ret = avcodec_open2(ctx, codec, &opts);
			if (ret < 0)
				throw std::runtime_error("Cannot open the audio codec: " + scy::av::averror(ret));
		}
		// Set the default input and output parameters are set here once the codec
		// context has been opened. The output pixel format, width or height can be
		// modified after this call and a conversion context will be created on the
		// next call to open() to output the desired format.
		initVideoCodecFromContext(stream, ctx, iparams);
		initVideoCodecFromContext(stream, ctx, oparams);

		// Default to bgr24 interleaved output.
		// oparams.pixelFmt = "bgr24";
	}
	
};
