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
#include <coredata/IOData.h>
#include <coredata/data_export.h>
#include <vector>

#pragma warning(push, 0) 
#include <opencv2/core/mat.hpp>
#pragma warning(pop) 

namespace data
{
	/**
	 * \brief ImageArrayData is the data type used to handle multiple images. Uses OpenCV.
	 */
	class DATA_EXPORT ImageArrayData : public IOData <Data, ImageArrayData>
	{
	protected:
		std::vector<cv::Mat> _array;	//<! container of all the images data. The data are handled by cv::Mat objects 

		ImageArrayData(IOData::_Protection priv) : IOData(SEQIMG)
		{

		}

		ImageArrayData(data::IODataType type) : IOData(type)
		{
			/*Data::registerInstance(new ImageArrayData());*/
		}

	public:

		/**
		 * \brief ImageArrayData default constructor, the internal IODataType data type will be "SEQIMG"
		 */
		ImageArrayData() : IOData(SEQIMG)
		{
			Data::registerInstance(new ImageArrayData(IOData::_Protection()));
			This()._type = SEQIMG;
		}

		ImageArrayData(const data::Data &right) : IOData(right.getType())
		{
			auto type = right.getType();
			if (type != SEQIMG && type!=SEQIMGD && type != IMGF) throw HipeException("ERROR data::ImageArrayData::ImageArrayData - Only Connexdata should call this constructor.type SEQIMG or SEQIMGD");
			Data::registerInstance(right);
			_array.resize(0);
			_decorate = true;
		}

		/**
		 * \brief ImageArrayData copy constructor
		 * \param right The ImageArrayData to copy data from
		 */
		ImageArrayData(const data::ImageArrayData &right) : IOData(right._type)
		{
			Data::registerInstance(right._This);
			_array.resize(0);
			_decorate = true;
		}

		virtual ~ImageArrayData()
		{
			IOData::release();
			_array.clear();
		}

		/**
		 * \brief Get the container of the images' data
		 * \return Returns a reference to the std::vector<cv::Mat> object containing the images' data
		 */
		std::vector<cv::Mat>& Array();

		/**
		 * \brief Get the container of the images' data (const version)
		 * \return Returns a constant reference to the std::vector<cv::Mat> object containing the images' data
		 */
		const std::vector<cv::Mat>& Array_const() const;

		/**
		 * \brief Add an image to the container.
		 * \param dataMat The image to add
		 * \return Returns a reference to the ImageArrayData object
		 */
		ImageArrayData& operator<<(cv::Mat dataMat);

		/**
		 * \brief Copy the images' data of the ImageArrayData object to another one
		 * \param left The other object where to copy the data to
		 */
		virtual void copyTo(ImageArrayData& left) const;

		/**
		 * \brief
		 * \return Returns true if the object doesn't contain any data
		 */
		bool empty() const;

		/**
		 * \todo
		 * \brief ImageArrayData assignment operator
		 * \param left The ImageArrayData oject to get the data from
		 * \return A reference to the object
		 */
		ImageArrayData& operator=(const ImageArrayData& left);
	};
}
