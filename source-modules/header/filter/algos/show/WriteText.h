//@HIPE_LICENSE@
#pragma once

#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class WriteText : public IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ImageArrayData, data::ImageArrayData);

			SET_NAMESPACE("vision/show");

			REGISTER(WriteText, ()), _connexData(data::INDATA)
			{
				isBottom_text = false;
				text = "";
			}

			REGISTER_P(bool, isBottom_text);

			REGISTER_P(std::string, text);

			
			~WriteText()
			{
			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		public:
			HipeStatus process();
		};

		ADD_CLASS(WriteText, isBottom_text, text);
	}
}
