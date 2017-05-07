#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <filter/data/IODataType.h>
#include <filter/data/IOData.h>

namespace filter
{
	namespace data
	{
		class ListIOData : public IOData<Data, ListIOData>
		{
		protected:
			std::vector<Data> _listIoData;
		public:
			ListIOData(std::vector<Data> listIoData) : IOData(IODataType::LISTIO)
			{
				_listIoData = listIoData;
			}

			std::vector<Data> getListIoData(){ return _listIoData; }
			
			ListIOData(const ListIOData& left)
			{
				_type = left._type;

				
			}
			void copyTo(ListIOData& left)
			{
				for (Data data : left._listIoData)
				{
					Data cur_data;
					data.copyTo(cur_data);

					_listIoData.push_back(cur_data);
				}
			}

			IOData& operator=(const ListIOData& left)
			{
				if (this == &left) return *this;

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

				for (auto& data : left._listIoData)
				{
					_listIoData.push_back(data);
				}

				return *this;
			}

			inline bool empty()
			{
				return _listIoData.empty();
			}

			std::vector<Data> & getListData()
			{
				return _listIoData;
			}
		};
	}
}
