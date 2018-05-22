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
