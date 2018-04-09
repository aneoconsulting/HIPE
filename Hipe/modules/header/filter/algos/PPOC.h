#pragma once
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>

#include <vector>

#include <data/ImageData.h>
#include <data/ImageArrayData.h>

#pragma warning(push, 0)
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class IPDesc;
		class PPOC : public filter::IFilter
		{
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			REGISTER(PPOC, ()), _connexData(data::INDATA)
			{
				_debug = false;
			}
			REGISTER_P(bool, _debug);

		public:
			HipeStatus process() override;

		private:
			cv::Mat preprocessBackgroundImage(const cv::Mat& image, const cv::Mat& background, cv::Mat& out_mask);
			cv::Mat matchImageToBackground(const cv::Mat & image, const cv::Mat & imageBackground);

			void findObject(const cv::Mat& referenceImage, const cv::Mat& queryImage, const cv::Mat& imageMask);
			bool isObjectPresent(const cv::Mat& queryImage, const IPDesc& poi);
			
			std::vector<std::vector<cv::Point>> findContours(const cv::Mat& imageGrayscale);
			std::vector<IPDesc> findROIs(cv::Mat& out_image, const cv::Mat& imageMask, const std::vector<std::vector<cv::Point>>& contours);
		
			bool isDescriptorNearImageBorder(const IPDesc& descriptor, const cv::Mat& image);
			bool isDescriptorParallel(const std::vector<cv::Point2f>& query, const std::vector<cv::Point2f>& roi);

			bool isOnROI(const std::vector<cv::Point2f>& kptsQuery, const IPDesc & poi);
		};

		ADD_CLASS(PPOC, _debug);

		class IPDesc
		{
		public:
			IPDesc(cv::Mat object, int pointX, int pointY, int pointWidth, int pointHeight)
				: corner_(cv::Rect(pointX, pointY, pointWidth, pointHeight)), descImg_(object)
			{

			}

			inline cv::Point corner() const { return cv::Point(corner_.x, corner_.y); }
			inline cv::Rect rect() const { return corner_; }
			inline int width() const { return corner_.width; }
			inline int height() const { return corner_.height; }
			inline cv::Mat descImg() const { return descImg_; }

		private:
			cv::Rect corner_;
			cv::Mat descImg_;
		};
	}
}
