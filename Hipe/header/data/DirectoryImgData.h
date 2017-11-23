#pragma once 
#include <data/ImageArrayData.h>
#include <boost/log/utility/setup/file.hpp>

#include <data/data_export.h>

namespace data
{
	/**
	 * \brief DirectoryImageData is the data type used to handle a collection of images contained in a folder. Uses OpenCV.
	 */
	class DATA_EXPORT DirectoryImgData : public IOData<ImageArrayData, DirectoryImgData>
	{
		/**
		 * \brief The path to the folder containing the images
		 */
		std::string _directoryPath;



			DirectoryImgData(IOData::_Protection priv) : IOData(IODataType::SEQIMGD)
			{

			}

	public:

			DirectoryImgData() : IOData(IODataType::SEQIMGD)
			{
				IOData::_Protection priv;
				Data::registerInstance(new DirectoryImgData(priv));

				This()._type = SEQIMGD;
			}
		/**
		 * \brief
		 * \param directoryPath The path to where the images are located
		 */

		DirectoryImgData(const std::string & directoryPath) : IOData(data::IODataType::SEQIMGD)
		{
			Data::registerInstance(new DirectoryImgData());

			This()._directoryPath = directoryPath;
			}

			//DirectoryImgData(const std::string & directoryPath, bool getImages) : IOData(data::IODataType::SEQIMGD)
			//{
			//	Data::registerInstance(new DirectoryImgData());

			//	This()._directoryPath = directoryPath;

			//	std::vector<cv::String> filenames;

			//	cv::glob(This()._directoryPath, filenames);

			//	for (size_t i = 0; i < filenames.size(); ++i)
			//	{
			//		cv::Mat mat = cv::imread(filenames[i]);

			//		if (mat.empty())
			//		{
			//			std::stringstream strbuild;
			//			strbuild << "Cannot open file : " <<  filenames[i];
			//			throw HipeException(strbuild.str());
			//		}

			//		cv::putText(mat,
			//			removeDirectoryName(filenames[i]),
			//			cv::Point(25, 25), // Coordinates
			//			cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
			//			1.0, // Scale. 2.0 = 2x bigger
			//			cv::Scalar(255, 255, 255), // Color
			//			1); // Anti-alias
			//		This()._array.push_back(mat);
			//	}
			//	if (This()._array.empty())
			//	{
			//		std::stringstream iss;
			//		iss << "No file loaded from directory : " << directoryPath;
			//		throw HipeException(iss.str());
			//	}
			//}


			/**
			* \brief Get the container of the images' data
			* \return Returns a reference to the std::vector<cv::Mat> object containing the images' data
			*/


			virtual DirectoryImgData& operator=(const DirectoryImgData& left)
			{
				Data::registerInstance(left);
				_type = left.getType();
				_decorate = left.getDecorate();

				return *this;
			}

			std::string DirectoryPath() const
			{
				return This_const()._directoryPath;
			}
			std::vector<cv::Mat> & images()
			{
				return This()._array;
			}

			/**
			* \brief Get data of an image by its index in the container
			* \return Returns a cv::Mat object containing the image' data
			*/
			cv::Mat image(int index)
			{
				return This()._array[index];
			}

			/**
			* \brief Does the request image contain data ?
			* \return Returns true if the request image doesn't contain any data
			*/
			inline bool empty() const
			{
				if (This_const()._directoryPath.empty() || This_const()._array.empty()) return true;
				return false;
			}

			void loadImagesData()
			{
			std::vector<cv::String> filenames;

			cv::glob(This()._directoryPath, filenames);

			for (size_t i = 0; i < filenames.size(); ++i)
			{
				cv::Mat mat = cv::imread(filenames[i]);

				if (mat.empty())
				{
					std::stringstream strbuild;
					strbuild << "Cannot open file : " << filenames[i];
					throw HipeException(strbuild.str());
				}

				cv::putText(mat,
					removeDirectoryName(filenames[i]),
					cv::Point(25, 25), // Coordinates
					cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
					1.0, // Scale. 2.0 = 2x bigger
					cv::Scalar(255, 255, 255), // Color
					1); // Anti-alias
				This()._array.push_back(mat);
			}
			if (This()._array.empty())
			{
				std::stringstream iss;
				iss << "No file loaded from directory : " << directoryPath;
				throw HipeException(iss.str());
			}


		}
		/**
		* \brief Get the container of the images' data
		* \return Returns a reference to the std::vector<cv::Mat> object containing the images' data
		*/
		inline std::vector<cv::Mat>& images();

		/**
		* \brief Get data of an image by its index in the container
		* \return Returns a cv::Mat object containing the image' data
		*/
		inline cv::Mat image(int index);
	};
}
