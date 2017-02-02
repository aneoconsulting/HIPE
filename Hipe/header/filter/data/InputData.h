#pragma once

#include <filter/data/IOData.h>
#include <opencv2/core/mat.hpp>

namespace filter {
	namespace data {
		class InputData : filter::data::IOData
		{
			

		public:
			InputData() 
			{
				
			}

			InputData(cv::Mat & input)
			{
				
			}

			InputData(filter::data::IOData & input)
			{

				_mat = input;
			}


		};
	}
}
