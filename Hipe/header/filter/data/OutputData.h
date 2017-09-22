#pragma once

#include <filter/data/IOData.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/opencv.hpp>
#include <core/base64.h>
#include <filter/data/ImageArrayData.h>

namespace filter {
	namespace data {
		/**
		 * \todo
		 * \brief [TODO]
		 */
		class OutputData : public IOData<ImageArrayData, OutputData>
		{
			std::string result;

		public:
			std::string getResult() const
			{
				return This_const().result;
			}

			void setResult(const std::string& result)
			{
				This().result = result;
			}


			OutputData() : IOData(IODataType::IMGF)
			{
			}

			OutputData(IODataType dataType) : IOData(dataType)
			{
			}

			OutputData& operator=(const Data& left)
			{
				if (left.getType() != IODataType::IMGF) throw HipeException("[ERROR] OutputData::operator= - data not of type IMGF");
				
				if (!_This) { _This.reset(); }
				Data::registerInstance(new OutputData());
				_type = IMGF;
				_decorate = true;
				This()._array = (static_cast<const ImageArrayData &>((left)).This_const().Array_const());

				return *this;
			}

			OutputData& operator=(const OutputData& left)
			{
				IOData::operator=(left);

				This().result = left.This_const().result;

				return *this;
			}

			std::string resultAsString() const
			{
				return This_const().result;
			};

			/**
			 * \brief extract the data of a cv::Mat image and convert it to base64 (as a string)
			 * \param m the input image ton convert
			 * \return the data of the input as an alphanumeric string
			 */
			static std::string mat2str(const cv::Mat& m)
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
				memcpy(&data[0 * sizeof(int)], reinterpret_cast<uchar*>(&src.rows), sizeof(int));
				memcpy(&data[1 * sizeof(int)], reinterpret_cast<uchar*>(&src.cols), sizeof(int));
				memcpy(&data[2 * sizeof(int)], reinterpret_cast<uchar*>(&type), sizeof(int));
				memcpy(&data[3 * sizeof(int)], reinterpret_cast<uchar*>(&channels), sizeof(int));

				// Add image data
				data.insert(data.end(), src.datastart, src.dataend);

				// Encode
				return base64_encode(data.data(), data.size());
			}

			boost::property_tree::ptree resultAsJson()
			{
				boost::property_tree::ptree resultTree;
				boost::property_tree::ptree outputTree;

				if (!_This)
				{
					outputTree.add<std::string>("info", "NO Data as response");
					resultTree.add_child("DataResult", outputTree);
					return resultTree;
				}

				This().result.clear();

				int data_index = 0;

				for (auto &input : Array())
				{
					std::string type = DataTypeMapper::getStringFromType(This().getType());

					std::stringstream key;
					key << "data_" << data_index;


					outputTree.add<std::string>("type", type);
					outputTree.add<std::string>(key.str(), mat2str(input));

					data_index++;
				}

				std::stringstream output;

				resultTree.add_child("DataResult", outputTree);

				return resultTree;
			}

			virtual void copyTo(OutputData& left) const
			{
				if (IOData::getType() != left.getType())
					throw HipeException("Cannot left argument in a ImageData");
				if (left.Array_const().size() > 1)
					throw HipeException("Number of images inside the source doesn't correspond to a ImageData");

				ImageArrayData::copyTo(static_cast<ImageArrayData &>(left));
			}

		};
	}
}
