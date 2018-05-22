//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once 
#include <data/ImageArrayData.h>
#include <data/ImageData.h>

#pragma warning(push, 0)
#include <boost/log/utility/setup/file.hpp>

#pragma warning(pop)


namespace data {
		/**
		* \brief DirectoryImageData is the data type used to handle a collection of images contained in a folder. Uses OpenCV.
		*/
		class DATA_EXPORT DirectoryImgData : public IOData<ImageArrayData, DirectoryImgData>
		{
			/**
			* \brief The path to the folder containing the images
			*/
			std::string _directoryPath;

			std::vector<cv::String> filenames;

		public:
			std::vector<cv::String> & getFilenames()
			{
				DirectoryImgData &ret = This();
				return ret.filenames;
			}

			void setFilenames(const std::vector<cv::String>& filenames)
			{
				DirectoryImgData &ret = This();
				ret.filenames = filenames;
				this->filenames = filenames;
			}

		private:
			int _idxFile;

			DirectoryImgData(IOData::_Protection priv) : IOData(IODataType::SEQIMGD)
			{
				_idxFile = 0;
			}


		public:
			//using IOData::IOData;

		public:

			DirectoryImgData() : IOData(IODataType::SEQIMGD)
			{
				IOData::_Protection priv;
				Data::registerInstance(new DirectoryImgData(priv));
				_idxFile = 0;
				This()._type = SEQIMGD;
				This()._idxFile = 0;
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

			void loadImagesData();

			void refreshDirectory();

			ImageData nextImageFile();

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

			//	
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
			void SetDirectoryPath(std::string path)
			{
				This()._directoryPath = path;
			}
			
			bool empty() const
			{
				if (This_const()._directoryPath.empty() || This_const()._array.empty()) return true;
				return false;
			}
			std::vector<cv::Mat>& images();
			cv::Mat image(int indew);

			bool hasFiles()
			{
				refreshDirectory();
				return (!This().filenames.empty());
			};
		};
	}

