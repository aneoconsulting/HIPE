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
#include <coredata/IODataType.h>

#include <coredata/IOData.h>
#include <data/ImageArrayData.h>
#include <coredata/data_export.h>
#pragma warning(push, 0) 
#include <opencv2/core/mat.hpp>
#pragma warning(pop)

namespace data
{
	/**
	 * \brief ImageData is the data type used to handle an image. Uses OpenCV.
	 */
	class DATA_EXPORT ImageData : public IOData<ImageArrayData, ImageData>
	{


	protected:
		ImageData(IOData::_Protection priv) : IOData(IMGF)
		{

		}

		ImageData(IODataType dataType) : IOData(dataType)
		{
			/*Data::registerInstance(new ImageData(IOData::_Protection()));
			This()._type = dataType;
			This()._array.resize(1);*/
		}

	public:

		/**
		 * \brief Default empty constructor
		 */
		ImageData() : IOData(IMGF)
		{
			Data::registerInstance(new ImageData(IOData::_Protection()));
			This()._type = IMGF;
			This()._array.resize(1);
		}

		/**
		 * \brief
		 * \param matrix The image's data
		 */
		ImageData(cv::Mat matrix) : IOData(IMGF)
		{
			Data::registerInstance(new ImageData(IOData::_Protection()));
			This()._type = IMGF;
			This()._array.resize(1);
			This()._array[0] = matrix;
		}

		ImageData(const ImageData & ref) : IOData(IMGF)
		{
			Data::registerInstance(ref._This);
			This()._type = ref.This_const()._type;
			_decorate = ref._decorate;

		}

		ImageData(const Data & ref) : IOData(IMGF)
		{
			if (ref.getType() != IMGF) 
				throw HipeException("ERROR data::ImageData::ImageData - Only Connexdata should call this constructor.");

			Data::registerInstance(ref);
			This()._type = ref.getType();
			_decorate = ref.getDecorate();
		}

		virtual ~ImageData()
		{

			IOData::release();
			_array.clear();
		}


		/**
		 * \brief Copy the image data of the ImageData object to another one.
		 * \param left The object where to copy the data to
		 */
		virtual void copyTo(ImageData& left) const;


		/**
		 * \brief Get the image's data
		 * \return Returns a reference to the cv::Mat object containing the image's data
		 */
		cv::Mat& getMat();

		/**
		* \brief Get the image's data (const version)
		* \return Returns a constant reference to the cv::Mat object containing the image's data
		*/
		const cv::Mat& getMat() const;

		/**
		* \brief
		* \return Returns true if the object doesn't contain any data
		*/
		bool empty() const;

		/*ImageData& operator=(const Data& left);

		ImageData& operator=(Data& left);*/
	};
}
