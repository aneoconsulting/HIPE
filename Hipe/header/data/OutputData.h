#pragma once

#include <data/IOData.h>
#include <opencv2/opencv.hpp>
#include <json/JsonTree.h>

#include <data/data_export.h>

namespace data
{
	/**
	 * \todo
	 * \brief [TODO]
	 */
	class DATA_EXPORT OutputData : public IOData<Data, OutputData>
	{
	
	private:
		Data input;
	
	public:
		OutputData() : IOData(IODataType::IMGB64)
		{

		}

		OutputData(const Data & ref) : IOData(ref.getType())
		{
			if (ref.getType() != IMGB64) throw HipeException("ERROR data::ImageArrayData::ImageArrayData - Only Connexdata should call this constructor.");

			Data::registerInstance(ref);
			
			input = ref;
			_decorate = true;
		}

		OutputData(IODataType dataType) : IOData(dataType)
		{

		}

		OutputData& operator=(const Data& left);

		OutputData& operator=(const OutputData& left);


		virtual void copyTo(OutputData& left) const;

		/**
		* \brief extract the data of a cv::Mat image and convert it to base64 (as a string)
		* \param m the input image ton convert
		* \return the data of the input as an alphanumeric string
		*/
		static std::string mat2str(const cv::Mat& m);

		Data getInput() const
		{
			return input;
		}

		bool empty()  const
		{
			if (_decorate == true)
				return This_const().empty();

			if (input.getType() == NONE) return true;

			return input.empty();
		}
	};
}
