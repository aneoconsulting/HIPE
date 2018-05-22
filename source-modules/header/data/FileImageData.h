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

#include <data/ImageData.h>
#include <coredata/IODataType.h>
#include <string>
#pragma warning(push, 0)
#include <opencv2/opencv.hpp>
#pragma warning(pop)

#include <coredata/data_export.h>


namespace boost {
	namespace filesystem {
	class path;
}
}


namespace data
{
	/**
	 * \brief FileImageData is the data type used to handle an image and additonnal information. Uses OpenCV.
	 */
	class DATA_EXPORT FileImageData : public IOData<ImageData, FileImageData>
	{
		/**
		 * \brief Path to the image
		 */
		std::shared_ptr<boost::filesystem::path> _filePath;

		inline cv::Mat asOutput();
	private:

		/**
		* \brief Default fileImage constructor for private usage only
		*/
		FileImageData();

	public:

		/**
		 * \brief FileImageData copy constructor
		 * \param right the FileImageData to copy
		 */
		FileImageData(const FileImageData& right) : IOData(IODataType::IMGF)
		{
			Data::registerInstance(right._This);
		}

		/**
		 * \brief Constructor with path to image
		 * \param filePath Complete path to the image
		 */
		FileImageData(const std::string& filePath);

		/**
		* \brief Constructor with raw or compressed data of image
		* \param data raw or compressed data in base64 of the image image
		*/
		FileImageData(const std::string& base64Data, const std::string& format, int width, int height, int channels);


		/**
		* \brief Copy the image data of the ImageData object to another one.
		* \param left The object where to copy the data to
		*/
		virtual void copyTo(ImageData& left) const;

		/**
		 * \todo
		 * \brief FileImageData assignment operator
		 * \param left The FileImageData object to get the data from
		 * \return A reference to the object
		 */
		FileImageData& operator=(const FileImageData& left);
	private:
		/**
		 * \brief Get the OpenCV data type corresponding to the image channels count needed to create a cv::Mat object (assuming the data type used is an unsigned char (8U))
		 * \param channels the image channels count
		 * \return the OpenCV value corresponding to a CV_8UCX image where x is the number of channels
		 */
		int getCV8UTypeFromChannels(int channels);
	};
}
