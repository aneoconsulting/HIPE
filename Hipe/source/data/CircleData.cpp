#include <data/CircleData.h>
#include <vector>

namespace data
{
	std::vector<cv::Vec3f> CircleData::getCircles()
	{
		return This()._circledata;
	}

	void CircleData::copyTo(CircleData& left) const
	{
		left.This()._circledata.clear();

		left.Add(*this, true);
	}

	void CircleData::Add(const CircleData& left, bool copy)
	{
		for (auto data : left._circledata)
		{
			cv::Vec3f cur_data = data;
			This()._circledata.push_back(cur_data);
		}
	}

	CircleData& CircleData::operator=(const CircleData& left)
	{
		if (this == &left) return *this;
		if (!_This) Data::registerInstance(new CircleData());

		This()._type = left._type;
		This()._circledata.clear();

		for (auto& iodata : left._circledata)
		{
			This()._circledata.push_back(iodata);
		}
		return *this;
	}

	IOData<Data, CircleData>& CircleData::operator<<(const CircleData& left)
	{
		if (_type != left._type)
			throw HipeException("Cannot add data because types are different");

		for (auto& data : left._circledata)
		{
			This()._circledata.push_back(data);
		}

		return *this;
	}

	bool CircleData::empty() const
	{
		return This_const()._circledata.empty();
	}

	const std::vector<cv::Vec3f>& CircleData::getCircles() const
	{
		return This_const()._circledata;
	}

	void CircleData::setCircles(const std::vector<cv::Vec3f>& left)
	{
		This()._circledata = left;
	}
}
