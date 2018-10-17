#include <algos/utils/FpsResolution.h>
#include "core/HipeTimer.h"

#include <core/Logger.h>
#include <core/HipeStatus.h>
#include "data/ImageData.h"
#include <opencv2/video.hpp>


HipeStatus algos::tools::FPSResolution::process()
{
	core::HipeTimer ti;
	if (!isStart)
	{
		sampler.start();
		data::ShapeData fps;
		PUSH_DATA(fps);
		isStart = true;
		return OK;
	}

	//Ok so new frame is coming. WHat's the time to process one frame ?
	sampler.stop();
	double elapse = sampler.getElapseTimeInMicro();
	
	double result = 1000000.0 / elapse; // nb Frame by 1 micro seconds;
	// Compute frame per second
	
	int fps = result;
	std::stringstream buildFPSString;
	buildFPSString << fps << " fps";

	//Ignore Data 

	while (_connexData.size() != 0)
	{
		data::Data data = _connexData.pop();
		if (data.getType() == data::IMGF)
		{
			data::ImageData & img = static_cast<data::ImageData &>(data);
			buildFPSString << " (" << img.getMat().rows << "x" << img.getMat().cols << ")";
		}
	}
	int baseline = 0;

	cv::Size text_size = cv::getTextSize(buildFPSString.str(), cv::HersheyFonts::FONT_HERSHEY_PLAIN, 0.8, 1, &baseline);

	data::ShapeData shape;
	cv::Rect line = cv::Rect(10, text_size.height + 30, text_size.width, 1);
	shape.add(line, cv::Scalar(255, 255, 255), buildFPSString.str());
	PUSH_DATA(shape);
	sampler.start();
	

	return OK;
}
