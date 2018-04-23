//@HIPE_LICENSE@
#pragma once
#include <deque>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterClass.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>
#include <data/ShapeData.h>

#pragma warning(push, 0)
#include <opencv2/core/mat.hpp>
#include <dlib/image_transforms/fhog.h>
#include <dlib/image_processing/correlation_tracker.h>
#pragma warning(pop)



namespace filter
{
	namespace algos
	{
		class FILTER_EXPORT IfStableTracking : public filter::IFilter
		{
			CONNECTOR(data::Data, data::ShapeData);

			REGISTER(IfStableTracking, ()), _connexData(data::INDATA)
			{
				_init = false;
				_id = -1;
				confidence = 5.f;
			}
			
			int _id;
			

			REGISTER_P(float, confidence);
			

			std::atomic<bool> _init;

			HipeStatus process() override;


		};

		ADD_CLASS(IfStableTracking, confidence);
	}
}
