#include <data/ListIOData.h>

namespace data
{
	std::vector<Data> ListIOData::getListIoData()
	{
		return This()._listIoData;
	}

	void ListIOData::copyTo(ListIOData& left) const
	{
		left.This()._listIoData.clear();

		left.Add(*this, true);
	}

	void ListIOData::Add(const ListIOData& left, bool copy)
	{
		for (Data data : left._listIoData)
		{
			Data cur_data;
			data.copyTo(cur_data);

			This()._listIoData.push_back(cur_data);
		}
	}

	ListIOData& ListIOData::operator=(const ListIOData& left)
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

	IOData<Data, ListIOData>& ListIOData::operator<<(const ListIOData& left)
	{
		if (_type != left._type)
			throw HipeException("Cannot add data because types are different");

		for (auto& data : left._listIoData)
		{
			This()._listIoData.push_back(data);
		}

		return *this;
	}

	bool ListIOData::empty() const
	{
		return This_const()._listIoData.empty();
	}

	const std::vector<Data>& ListIOData::getListData() const
	{
		return This_const()._listIoData;
	}

	void ListIOData::setListData(const std::vector<Data>& left)
	{
		This()._listIoData = left;
	}
}
