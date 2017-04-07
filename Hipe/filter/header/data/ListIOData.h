#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <filter/data/IODataType.h>
#include <filter/data/IOData.h>

namespace filter
{
	namespace data
	{
		class ListIOData : public IOData
		{
		protected:
			std::vector<IOData> _listIoData;
		public:
			ListIOData(std::vector<IOData> listIoData) : IOData(IODataType::LISTIO)
			{
				_listIoData = listIoData;
			}
			std::vector<cv::Mat>& getInputData()
			{
				throw HipeException("getInputData is not implemented (ListioData)");
			}

			void setInputData(const std::vector<cv::Mat>& mats)
			{
				throw HipeException("setInputData is not implemented (ListioData)");
			}

			cv::Mat& getInputData(int index)
			{
				throw HipeException("getInputData(index) is not implemented (ListioData)");
			}

			void addInputData(cv::Mat& mat)
			{
				throw HipeException("addInputData is not implemented (ListioData)");
			}

			ListIOData(const ListIOData& left, bool copy)
			{
				_type = left._type;
				_data.clear();
				for (IOData& iodata : left._listIoData)
				{
					IOData cur_iodata;
					if (copy) iodata.copyTo(cur_iodata);
					else cur_iodata = iodata;
					_listIoData.push_back(cur_iodata);
				}
			}
			void copyTo(ListIOData& left)
			{
				ListIOData res(*this, true);

				left = res;
			}

			void copyRefTo(ListIOData& left)
			{
				ListIOData res(*this, false);

				left = res;
			}


			IOData& operator=(const ListIOData& left)
			{
				_type = left._type;
				_listIoData.clear();

				for (auto& iodata : left._listIoData)
				{
					_listIoData.push_back(iodata);
				}
				return *this;
			}

			IOData& operator<<(const ListIOData& left)
			{
				if (_type != left._type)
					throw HipeException("Cannot add data because types are different");

				for (auto& mat : left._data)
				{
					_data.push_back(mat);
				}
				return *this;
			}

			inline bool empty()
			{
				return _listIoData.empty();
			}

			template<typename type>
			inline static type & downCast(data::IOData & outputData)
			{
				throw HipeException("Not yet implemented");
			}
		};
	}
}
