//@HIPE_LICENSE@
#pragma once 
#include <boost/log/utility/setup/file.hpp>
#include <coredata/NoneData.h>
#include <coredata/IODataType.h>
#include <data/ImageArrayData.h>
#include <data/ImageData.h>
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
		class FILTER_EXPORT DirectoryImgDataSource : public filter::IFilter
		{
			CONNECTOR(data::NoneData, data::Data);

			SET_NAMESPACE("vision/datasource");

			REGISTER(DirectoryImgDataSource, ()), _connexData(data::INDATA)
			{
				eSourceType = data::IODataType::SEQIMGD;
				loop = false;
				atomic_state = false;
			}


			REGISTER_P(std::string, sourceType);

			REGISTER_P(bool, loop);
			REGISTER_P(std::string, directoryPath);


			data::IODataType eSourceType;

			data::ImageArrayData imgs;

			std::atomic<bool> atomic_state;

			HipeStatus process();
		};

		ADD_CLASS(DirectoryImgDataSource, sourceType, loop, directoryPath);

	}
}
#pragma once
