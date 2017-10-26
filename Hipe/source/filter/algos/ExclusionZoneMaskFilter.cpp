#include <filter/algos/ExclusionZoneMaskFilter.h>

HipeStatus filter::algos::ExclusionZoneMaskFilter::process()
{
	data::ImageData data = _connexData.pop();
	cv::Mat& image = data.getMat();

	if (!image.data) throw HipeException("Error - filter::algos::ExclusionZoneMaskFilter::process - no input data found.");

	compute_exclusion_zone_mask(image);

	return OK;
}

void filter::algos::ExclusionZoneMaskFilter::compute_exclusion_zone_mask(cv::Mat & image)
{
	// Collect parameters here for easier exposure later if desired.
	double clahe_clipLimit = 1.0;
	cv::Size clahe_tileGridSize{ 4,4 };
	double denoise_h = 20;
	int denoise_templateWindowSize = 20;
	int denoise_searchWindowSize = 40;

	cv::Mat mask_image;
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clahe_clipLimit, clahe_tileGridSize);

	//auto t1 = cv::getTickCount();
	cv::fastNlMeansDenoisingColored(image, mask_image, denoise_h, denoise_templateWindowSize, denoise_searchWindowSize);
	//auto t2 = cv::getTickCount();
	//auto time = (t2 - t1) / cv::getTickFrequency();
	cv::cvtColor(mask_image, mask_image, cv::COLOR_BGR2GRAY);
	clahe->apply(mask_image, mask_image);
	cv::threshold(mask_image, image, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}