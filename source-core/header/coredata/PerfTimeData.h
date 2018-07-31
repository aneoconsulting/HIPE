//@HIPE_LICENSE@
#pragma once
#include <coredata/IOData.h>
#include <core/HipeTimer.h>


namespace data
{

	class DATA_EXPORT PerfTimeData : public IOData<Data, PerfTimeData>
	{
		core::HipeTimer hipeTimer;
		double sampling;

	public:
		double getSampling() const
		{
			return This_const().sampling;
		}

		void setSampling(const double sampling)
		{
			This().sampling = sampling;
		}

		core::HipeTimer getHipeTimer() const
		{
			return This_const().hipeTimer;
		}

	
	private:
		PerfTimeData(IOData::_Protection priv) : IOData(IODataType::TIMER)
		{

		}

	public:
		PerfTimeData() : IOData(IODataType::TIMER)
		{
			Data::registerInstance(new PerfTimeData(IOData::_Protection()));
		}
		PerfTimeData(core::HipeTimer i_hipeTimer) : IOData(IODataType::TIMER)
		{
			Data::registerInstance(new PerfTimeData(IOData::_Protection()));
			This().hipeTimer = i_hipeTimer;

		}

		PerfTimeData(const PerfTimeData &data) : IOData(data._type)
		{
			Data::registerInstance(data._This);

		}

		PerfTimeData & operator=(const Data &data)
		{
			if (data.getType() != IODataType::TIMER)
				throw HipeException("the left assignment differ from timer");
			const PerfTimeData & perfCast = static_cast<const PerfTimeData &>(data);

			Data::registerInstance(perfCast._This);


			return *this;
		}
	};
}
