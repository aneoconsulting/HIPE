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
#include <coredata/Data.h>

#include <json/JsonTree.h>

#include <coredata/data_export.h>

namespace cv
{
	class Mat;
}

namespace data
{
	/**
	 * \todo
	 * \brief [TODO]
	 */
	class DATA_EXPORT OutputData : public IOData<Data, OutputData>
	{
	private:
		Data input;

	public:
		OutputData() : IOData(IODataType::IMGB64)
		{
		}

		OutputData(const Data& ref) : IOData(ref.getType())
		{
			if (ref.getType() != IMGB64) throw HipeException("ERROR data::ImageArrayData::ImageArrayData - Only Connexdata should call this constructor.");

			Data::registerInstance(ref);

			input = ref;
			_decorate = true;
		}

		OutputData(IODataType dataType) : IOData(dataType)
		{
		}

		OutputData& operator=(const Data& left);

		OutputData& operator=(const OutputData& left);


		virtual void copyTo(OutputData& left) const;

		/**
		* \brief extract the data of a cv::Mat image and convert it to base64 (as a string)
		* \param m the input image ton convert
		* \return the data of the input as an alphanumeric string
		*/
		static std::string mat2str(const cv::Mat& m);

		Data getInput() const
		{
			return input;
		}

		bool empty() const
		{
			if (_decorate == true)
				return This_const().empty();

			if (input.getType() == NONE) return true;

			return input.empty();
		}
	};
}
