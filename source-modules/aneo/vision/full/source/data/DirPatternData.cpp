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

#include <DirPatternData.h>
#include <coredata/IODataType.h>
#include <coredata/IOData.h>
#include <data/ImageData.h>
#include <data/VideoData.h>
#include <data/DirectoryImgData.h>

namespace data
{



	/**
	* \brief Overloaded ssignment operator used to copy PatternData objects.
	* \param left The PatternData object to use as a source for the copy
	* \return Returns a reference to the copied PatternData object
	*/
	DirPatternData& DirPatternData::operator=(const DirPatternData& left)
	{
		if (_This == left._This) return *this;

		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}

	/**
	* \todo
	* \brief Overloaded insersion operator used to copy PatternData objects.
	* \param left The PatternData object to use as a source for the copy
	* \return Returns a reference to the copied PatternData object
	*/
	DirPatternData& DirPatternData::operator<<(const DirPatternData& left)
	{
		if (_This == left._This) return *this;

		Data::registerInstance(left._This);

		return *this;
	}

	/**
	* \brief Overloaded insetion operator used to overwrite the PatternData object's request image with another one
	* \param left The ImageData used to overwrite The PatternData's one
	* \return Returns a reference to the copied PatternData objet
	*/
	DirPatternData& DirPatternData::operator<<(const ImageData& left)
	{
		This()._inputSource = static_cast<Data>(left);
		return *this;
	}


	Data DirPatternData::imageSource() const
	{
		return This_const()._inputSource;
	}

	DirectoryImgData DirPatternData::DirectoryImg() const
	{
		return This_const().dir;
	}

	
	void DirPatternData::copyTo(DirPatternData& left) const
	{
		left.This()._inputSource = This_const()._inputSource;
		left.This().dir = This_const().dir;
	}


	
	bool DirPatternData::empty() const
	{
		if (This_const()._inputSource.empty() && This_const().dir.empty()) return true;

		return false;
	}

	DirPatternData & DirPatternData::operator=(const Data& left)
	{
		Data::registerInstance(left);
		_type = left.getType();
		_decorate = left.getDecorate();

		return *this;
	}
}

