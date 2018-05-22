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

#include <coredata/OutputData.h>
#include <core/base64.h>
#include <vector>

#pragma warning(push, 0)
#include <opencv2/opencv.hpp>
#pragma warning(pop)

namespace data
{
	OutputData& OutputData::operator=(const Data& left)
	{
		if (left.getType() == IODataType::IMGB64)
		{
			IOData::operator=(left);

			return *this;
		}

		if (left.getType() != IODataType::IMGF && left.getType() != IODataType::IMGENC) throw HipeException("[ERROR] OutputData::operator= - data not of type IMGF");

		if (_This)
		{
			_This.reset();
		}

		Data::registerInstance(new OutputData());
		_decorate = true;
		input = left;
		This().input = left;

		return *this;
	}

	OutputData& OutputData::operator=(const OutputData& left)
	{
		IOData::operator=(left);



		return *this;
	}



	std::string OutputData::mat2str(const cv::Mat& m)
	{
		cv::Mat src;
		if (!m.isContinuous())
		{
			src = m.clone();
		}
		else
		{
			src = m;
		}

		std::vector<uchar> data(src.datastart, src.dataend);

		// Encode
		return base64_encode(data.data(), data.size());
	}



	void OutputData::copyTo(OutputData& left) const
	{
		if (IOData::getType() != left.getType())
			throw HipeException("Cannot left argument in a ImageData");


		left.This().input = This_const().input;
	}
}
