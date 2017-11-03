#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include "data/ImageArrayData.h"
#include "data/ImageData.h"



namespace filter
{
	namespace algos
	{

		class FingerPrintMinutia : public filter::IFilter
		{

			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageData);

			REGISTER(FingerPrintMinutia, ()), _connexData(data::INDATA)
			{
				minHessian = 400;
				matchthreshold = 0.1;
				matchcoeff = 2;

			}

			REGISTER_P(int, minHessian);
			REGISTER_P(double, matchthreshold);
			REGISTER_P(double, matchcoeff);


		public:
			HipeStatus process() override;
			void thinning(cv::Mat &im);
			void thinningIteration(cv::Mat& im, int iter);
			void getDescriptor(cv::Mat const im, cv::Mat& descriptor, std::vector<cv::KeyPoint>  & keypoints);

		};

		ADD_CLASS(FingerPrintMinutia, minHessian, matchthreshold,matchcoeff);
	}
}
#pragma once
