//@HIPE_LICENSE@
#include <algos/extraction/ExtractBackground.h>
#include <opencv2/video/background_segm.hpp>
#include <algos/extraction/ExctractSubImage.h>


HipeStatus filter::algos::ExtractBackground::process()
{
	data::ImageData data = _connexData.pop();

	if (!background_subtractor_mog2)
	{
		background_subtractor_mog2 = cv::createBackgroundSubtractorMOG2(history_frames, varThreshold, false);
		
	}
	cv::Mat input;
	cv::Mat result;
	input = data.getMat();

	background_subtractor_mog2->apply(input, result);
	

	PUSH_DATA(data::ImageData(result));

	return OK;
}
