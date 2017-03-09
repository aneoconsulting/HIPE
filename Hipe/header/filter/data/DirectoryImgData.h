#include "InputArrayData.h"
#include <boost/log/utility/setup/file.hpp>

namespace filter {
	namespace Algos {
		class DirectoryImgData : public filter::Algos::InputArrayData
		{
			std::string _directoryPath;
		public:
			DirectoryImgData(const std::string & directoryPath) : InputArrayData(data::IODataType::SEQIMGD)
			{
				
				_directoryPath = directoryPath;
				std::vector<cv::String> filenames;

				cv::glob(_directoryPath, filenames); 

				for (size_t i = 0; i < filenames.size(); ++i)
				{
					cv::Mat mat = cv::imread(filenames[i]);

					if (mat.empty())
					{
						std::stringstream strbuild;
						strbuild << "Cannot open file : " <<  filenames[i];
						throw HipeException(strbuild.str());
					}
					addInputData(mat);
				}
				if (_data.empty())
				{
					std::stringstream iss;
					iss << "No file loaded from directory : " << directoryPath;
					throw HipeException(iss.str());
				}
			}			
		};
	}
}
