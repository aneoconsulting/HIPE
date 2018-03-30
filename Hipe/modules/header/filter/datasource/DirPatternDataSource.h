#pragma once 
#include <boost/log/utility/setup/file.hpp>
#include <coredata/NoneData.h>
#include <coredata/IODataType.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
#include <data/DirPatternData.h>
#include <coredata/ConnexData.h>
#include <corefilter/tools/filterMacros.h>
#include <corefilter/tools/RegisterTools.hpp>
#include <corefilter/tools/RegisterTable.h>
#include <core/HipeStatus.h>
#include <corefilter/Model.h>
#include <corefilter/IFilter.h>
#include <corefilter/filter_export.h>

#include <string>
#include <data/FileImageData.h>
#include <data/VideoData.h>
#include <data/DirectoryImgData.h>

namespace filter
{
	namespace datasource
	{
		class FILTER_EXPORT DirPatternDataSource : public filter::IFilter
		{
			int _endOfSource;
			CONNECTOR(data::Data, data::DirPatternData);

			SET_NAMESPACE("vision/datasource");

			REGISTER(DirPatternDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::DIRPATTERN;
				loop = false;
				atomic_state = false;
				_endOfSource = -1;
			}

			REGISTER_P(bool, loop);

			data::IODataType eSourceType;
			std::atomic<bool> atomic_state;
			HipeStatus process();
		};

		ADD_CLASS(DirPatternDataSource, loop);

	}
}
#pragma once
