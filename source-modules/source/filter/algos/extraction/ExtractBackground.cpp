//@HIPE_LICENSE@
#include <algos/extraction/ExtractBackground.h>
#include <opencv2/video/background_segm.hpp>
#include "algos/extraction/ExctractSubImage.h"


HipeStatus filter::algos::ExtractBackground::process()
{
	data::ImageData data = _connexData.pop();

	if (!background_subtractor_mog2)
	{
		background_subtractor_mog2 = cv::cuda::createBackgroundSubtractorMOG();
		
	}
	cv::cuda::GpuMat input;
	cv::cuda::GpuMat result;
	input.upload(data.getMat());

	background_subtractor_mog2->apply(input, result);
	cv::Mat h_result;
	result.download(h_result);

	PUSH_DATA(data::ImageData(h_result));

	return OK;
}
