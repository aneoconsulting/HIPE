//@HIPE_LICENSE@
#pragma once

#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <corefilter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
#include <data/ShapeData.h>
#pragma warning(push, 0)
#include <opencv2/highgui/highgui.hpp>
#include <stack>

#pragma warning(pop)


namespace filter
{
	namespace algos
	{
		class TextLoggerShow : public IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::ShapeData, data::ImageData);

			SET_NAMESPACE("vision/show");

			REGISTER(TextLoggerShow, ()), _connexData(data::INDATA)
			{
				index = 0;
				fontscale = 0.8;
				height = 720;
				width = 1024;
			}



			REGISTER_P(double, fontscale);
			REGISTER_P(int, width);
			REGISTER_P(int, height);
			

			std::deque<std::string> texts;
			std::deque<size_t> text_height;
			std::deque<size_t> text_width;
			int index;


			~TextLoggerShow()
			{
			}

			virtual std::string resultAsString()
			{
				return std::string("TODO");
			};

		public:
			size_t computeSizeOfAllText();
			size_t getNextPosition(int i);
			HipeStatus process();
		};

		ADD_CLASS(TextLoggerShow, fontscale, width, height);
	}
}
