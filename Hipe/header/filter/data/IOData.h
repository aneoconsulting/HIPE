#pragma once
#include <opencv2/core/mat.hpp>

namespace filter {
	namespace data {
		class IOData
		{
			std::vector<cv::Mat> _data;
		public:
			std::vector<cv::Mat> & getData()
			{
				return _data;
			}

			void setData(const std::vector<cv::Mat> & mats)
			{
				_data = mats;
			}

			cv::Mat & getData(int index)
			{
				return _data[index];
			}

			void setData(cv::Mat & mat, int index)
			{
				_data.push_back(mat);
			}
		};
	}
}
