#include <filter/data/ImageArrayData.h>
#include <boost/log/utility/setup/file.hpp>

namespace filter {
	namespace data {
		class DirectoryImgData : public IOData<ImageArrayData, DirectoryImgData>
		{
			std::string _directoryPath;
		
			DirectoryImgData() : IOData(data::IODataType::SEQIMGD)
			{
				
			}

		public:
			DirectoryImgData(const std::string & directoryPath) : IOData(data::IODataType::SEQIMGD)
			{
				Data::registerInstance(new DirectoryImgData());

				This()._directoryPath = directoryPath;
				std::vector<cv::String> filenames;

				cv::glob(This()._directoryPath, filenames);

				for (size_t i = 0; i < filenames.size(); ++i)
				{
					cv::Mat mat = cv::imread(filenames[i]);

					if (mat.empty())
					{
						std::stringstream strbuild;
						strbuild << "Cannot open file : " <<  filenames[i];
						throw HipeException(strbuild.str());
					}
					This()._array.push_back(mat);
				}
				if (This()._array.empty())
				{
					std::stringstream iss;
					iss << "No file loaded from directory : " << directoryPath;
					throw HipeException(iss.str());
				}

				
			}	
			
			std::vector<cv::Mat> & images()
			{
				return This()._array;
			}

			cv::Mat image(int index)
			{
				return This()._array[index];
			}
		};
	}
}
