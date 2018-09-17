//%HIPE_LICENSE%

#include <algos/preprocessing/Resize.h>

HipeStatus filter::algos::Resize::process()
{
	while (!_connexData.empty()) // While i've parent data
	{
		data::ImageArrayData outImgArray;
		auto images = _connexData.pop();
		if (images.getType() == data::PATTERN)
		{
			throw HipeException("The resize object cant resize PatternData. Please Develop ResizePatterData");
		}

		//Resize all images coming from the same parent
		for (auto& myImage : images.Array())
		{
			if (myImage.empty())
				continue;
			cv::Mat& imgWork = myImage;
			if (!inoutData)
				imgWork = myImage.clone();

			if (width == 0 || height == 0)
			{
				int l_iwidth = imgWork.cols;
				int l_iheight = imgWork.rows;
				cv::Size size(l_iwidth / ratio, l_iheight / ratio);

				cv::resize(imgWork, imgWork, size, 0.0, 0.0, cv::INTER_CUBIC);
			}
			else
			{
				cv::Size size(width, height);
				cv::resize(imgWork, imgWork, size, 0.0, 0.0, cv::INTER_CUBIC);
			}

			if (!inoutData)
			{
				outImgArray.Array().push_back(imgWork);
			}
		}

		if (!inoutData)
		{
			PUSH_DATA(outImgArray);
		}
		else
		{
			PUSH_DATA(images);
		}
	}
	return OK;
}
