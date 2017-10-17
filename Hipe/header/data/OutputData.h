#pragma once

#include <data/IOData.h>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <opencv2/opencv.hpp>
#include <core/base64.h>
#include <data/ImageArrayData.h>
#include "ImageEncodedData.h"

#include <data/data_export.h>

namespace data
{
	/**
	 * \todo
	 * \brief [TODO]
	 */
	class DATA_EXPORT OutputData : public IOData<Data, OutputData>
	{
		std::string result;
		Data input;
		//std::shared_ptr<Data> input;
	public:
		inline std::string getResult() const;

		inline void setResult(const std::string& result);


		OutputData() : IOData(IODataType::IMGB64)
		{

		}

		OutputData(IODataType dataType) : IOData(dataType)
		{

		}

		OutputData& operator=(const Data& left);

		OutputData& operator=(const OutputData& left);

		inline std::string resultAsString() const;;

		/**
		 * \brief extract the data of a cv::Mat image and convert it to base64 (as a string)
		 * \param m the input image ton convert
		 * \return the data of the input as an alphanumeric string
		 */
		static std::string mat2str(const cv::Mat& m);

		boost::property_tree::ptree resultAsJson();

		virtual void copyTo(OutputData& left) const;
	};
}
