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

			using IOData::IOData;

			ListIOData() : IOData(IODataType::LISTIO)
			{
				
			}

		public:
			ListIOData(std::vector<Data> listIoData) : IOData(IODataType::LISTIO)
			{
				Data::registerInstance(new ListIOData());
				This()._listIoData = listIoData;
			}

			std::vector<Data> getListIoData(){ return This()._listIoData; }
			
			ListIOData(const ListIOData& left) : IOData(left)
			{
				This()._listIoData = left._listIoData;
			}

			void copyTo(ListIOData& left) const
			{
				left.This()._listIoData.clear();

				left.Add(*this, true);
			}

			void Add(const ListIOData& left, bool copy = false)
			{
				for (Data data : left._listIoData)
				{
					Data cur_data;
					data.copyTo(cur_data);

					This()._listIoData.push_back(cur_data);
				}
			}

			virtual ListIOData& operator=(const ListIOData& left)
			{
				if (this == &left) return *this;
				if (!_This) Data::registerInstance(new ListIOData());

				This()._type = left._type;
				This()._listIoData.clear();

				for (auto& iodata : left._listIoData)
				{
					This()._listIoData.push_back(iodata);
				}
				return *this;
			}

			virtual IOData& operator<<(const ListIOData& left)
			{
				if (_type != left._type)
					throw HipeException("Cannot add data because types are different");

				for (auto& data : left._listIoData)
				{
					This()._listIoData.push_back(data);
				}

				return *this;
			}

			inline bool empty() const
			{
				return This_const()._listIoData.empty();
			}

			const std::vector<Data> & getListData() const
			{
				return This_const()._listIoData;
			}

			void setListData(const std::vector<Data> &left) 
			{
				This()._listIoData = left;
			}
		};
	}
}
