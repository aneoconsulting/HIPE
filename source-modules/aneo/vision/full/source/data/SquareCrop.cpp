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

#include <data/SquareCrop.h>

namespace data
{
	std::vector<cv::Rect> SquareCrop::getSquareCrop() const
	{
		return This_const()._squareCrop;
	}

	ImageData SquareCrop::getPicture() const
	{
		return This_const()._picture;
	}

	SquareCrop& SquareCrop::operator=(const SquareCrop& left)
	{
		if (this == &left) return *this;
		IOData::_Protection priv;
		if (!left._This) Data::registerInstance(new SquareCrop(priv));
		else
			Data::registerInstance(left._This);


		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const std::vector<int>& left)
	{
		if (left.size() % 4 != 0)
		{
			std::stringstream strbuild;
			strbuild << "Cannot push the list of crop because input " << left.size() << " isn't a modulo of 4 (2 positions X,Y and 2 size width,height)";
			throw HipeException(strbuild.str());
		}

		for (unsigned int index = 0; index < left.size(); index += 4)
		{
			cv::Rect rect(left[index], left[index + 1], left[index + 2], left[index + 3]);
			This()._squareCrop.push_back(rect);
		}

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const std::vector<cv::Rect>& left)
	{
		This()._squareCrop = left;

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::addPair(const std::vector<cv::Point>& leftCrop, const ImageData& leftImage)
	{
		if (leftCrop.size() % 2 != 0)
		{
			std::stringstream strbuild;
			strbuild << "Cannot push the list of crop because input " << leftCrop.size() << " isn't a modulo of 2";
			throw HipeException(strbuild.str());
		}

		for (unsigned int index = 0; index < leftCrop.size(); index += 2)
		{
			cv::Rect rect(leftCrop[index], leftCrop[index + 1]);
			This()._squareCrop.push_back(rect);
		}

		This()._picture = leftImage;
		This().crops(true);

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const std::vector<cv::Point>& left)
	{
		if (left.size() % 2 != 0)
		{
			std::stringstream strbuild;
			strbuild << "Cannot push the list of crop because input " << left.size() << " isn't a modulo of 2";
			throw HipeException(strbuild.str());
		}

		for (unsigned int index = 0; index < left.size(); index += 2)
		{
			cv::Rect rect(left[index], left[index + 1]);
			This()._squareCrop.push_back(rect);
		}

		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const ImageData& left)
	{
		if (left.empty()) throw HipeException("No more Image to add in SquareCrop");

		This()._picture = left;


		return *this;
	}

	IOData<Data, SquareCrop>& SquareCrop::operator<<(const cv::Mat left)
	{
		if (left.empty()) throw HipeException("No more Image to add in SquareCrop");

		This()._picture = ImageData(left);


		return *this;
	}

	std::vector<cv::Mat>& SquareCrop::crops(bool forceRefresh)
	{
		if (forceRefresh == false &&
			This_const()._cropCache.size() == This_const()._squareCrop.size())
		{
			return This()._cropCache;
		}

		std::vector<cv::Mat> res;

		for (cv::Rect crop : This()._squareCrop)
		{
			res.push_back(This()._picture.getMat()(crop));
		}

		This()._cropCache = res;

		return This()._cropCache;
	}
}
