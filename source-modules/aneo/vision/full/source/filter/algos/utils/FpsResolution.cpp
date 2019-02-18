#include <algos/utils/FpsResolution.h>
#include "core/HipeTimer.h"

#include <core/Logger.h>
#include <core/HipeStatus.h>
#include "data/ImageData.h"
#include <opencv2/video.hpp>


HipeStatus algos::tools::FPSResolution::process()
{
	core::HipeTimer ti;

	//Ignore Data 
	data::ImageData img;
	std::stringstream buildFPSString;
	while (_connexData.size() != 0)
	{
		img = _connexData.pop();
		if (img.getType() == data::IMGF)
		{
			buildFPSString << " (" << img.getMat().cols << "x" << img.getMat().rows << ")";
		}
	}
	if (!isStart)
	{
		sampler.start();
		
		PUSH_DATA(img);
		isStart = true;
		return OK;
	}

	//Ok so new frame is coming. WHat's the time to process one frame ?
	
	double elapse = sampler.stop().getElapseTimeInMili();
	sampler.start();

	double result = (1.0 / elapse) * 1000.0; // nb Frame by 1 micro seconds;
	// Compute frame per second
	
	int fps = result;
	
	buildFPSString << fps << " fps";

	std::cout << "INFO: " << buildFPSString.str() << std::endl;

	
	int baseline = 0;

	cv::Size text_size = cv::getTextSize(buildFPSString.str(), cv::HersheyFonts::FONT_HERSHEY_PLAIN, 0.8, 1, &baseline);

	//data::ShapeData shape;
	cv::Rect line = cv::Rect(10, text_size.height + 30, text_size.width, 1);
	//shape.add(line, cv::Scalar(255, 255, 255), buildFPSString.str());
	cv::putText(img.getMat(), buildFPSString.str(), cv::Point(line.x, text_size.height + 10),
		            cv::HersheyFonts::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255), 2);
	PUSH_DATA(img);
	
	

	return OK;
}
