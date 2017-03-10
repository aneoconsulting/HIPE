#pragma once

#include <filter/data/IOData.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/opencv.hpp>
#include <filter/tools/base64.h>

namespace filter {
	namespace data {
		class OutputData : public IOData
		{
			std::string result;
		public:
			std::string getResult() const
			{
				return result;
			}

			void setResult(const std::string& result)
			{
				this->result = result;
			}

			
			OutputData() : IOData(IODataType::IMGF)
			{
			}

			OutputData(IODataType dataType) : IOData(dataType)
			{
			}

			OutputData(const IOData & in) : IOData(in)
			{
			}

			OutputData(const OutputData & in) : IOData(in)
			{
				result = in.result;
			}

			OutputData& operator=(const OutputData& left)
			{
				IOData::operator=(left);

				result = left.result;

				return *this;
			}

			std::string resultAsString()
			{
				return result;
			};
			
			std::string mat2str(const cv::Mat& m)
			{
				cv::Mat src;
				if (!m.isContinuous()) {
					src = m.clone();
				}
				else {
					src = m;
				}

				// Create header
				int type = m.type();
				int channels = m.channels();
				std::vector<uchar> data(4 * sizeof(int));
				memcpy(&data[0 * sizeof(int)], (uchar*)&src.rows, sizeof(int));
				memcpy(&data[1 * sizeof(int)], (uchar*)&src.cols, sizeof(int));
				memcpy(&data[2 * sizeof(int)], (uchar*)&type, sizeof(int));
				memcpy(&data[3 * sizeof(int)], (uchar*)&channels, sizeof(int));

				// Add image data
				data.insert(data.end(), src.datastart, src.dataend);

				// Encode
				return base64_encode(data.data(), data.size());
			}

			boost::property_tree::ptree resultAsJson()
			{
				result.clear();

				boost::property_tree::ptree resultTree;
				boost::property_tree::ptree outputTree;
				int data_index = 0;

				for (auto &input : getInputData())
				{
					std::stringstream key;
					key << "data_" << data_index;

					outputTree.add<std::string>(key.str(), mat2str(input));

					data_index++;
				}

				std::stringstream output;

				//

				resultTree.add_child("DataResult", outputTree);

				return resultTree;

			};
			

		};
	}
}
