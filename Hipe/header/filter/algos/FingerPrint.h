#pragma once
#include <core/HipeStatus.h>
#include <filter/tools/RegisterClass.h>
#include <filter/IFilter.h>

#include <data/ImageArrayData.h>
#include <data/ImageData.h>

#include <cstdio>

namespace filter
{
	namespace algos
	{
		class FingerPrint : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageData);

			REGISTER(FingerPrint, ()), _connexData(data::INDATA)
			{
				minHessian = 400;
				matchthreshold = 0.1;
				matchcoeff = 2;
				minnumbermatch = 4;

			}

			REGISTER_P(int, minHessian);
			REGISTER_P(double, matchthreshold);
			REGISTER_P(double, matchcoeff);
			REGISTER_P(double, minnumbermatch);


		public:
			HipeStatus process() override;

		};

		ADD_CLASS(FingerPrint, minHessian, matchthreshold,matchcoeff, minnumbermatch);
	}
}
#pragma once
