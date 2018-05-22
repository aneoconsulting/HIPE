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

#include <filter/algos/preprocessing/Erode.h>

int filter::algos::Erode::convertMorphType(const std::string & name)
{
	if (name == "BLACKHAT")
	{
		return cv::MorphTypes::MORPH_BLACKHAT;
	}
	else if (name == "CLOSE")
	{
		return cv::MorphTypes::MORPH_CLOSE;
	}
	else if (name == "DILATE")
	{
		return cv::MorphTypes::MORPH_DILATE;
	}
	else if (name == "ERODE")
	{
		return cv::MorphTypes::MORPH_ERODE;
	}
	else if (name == "GRADIENT")
	{
		return cv::MorphTypes::MORPH_GRADIENT;
	}
	else if (name == "HITMISS")
	{
		return cv::MorphTypes::MORPH_HITMISS;
	}
	else if (name == "OPEN")
	{
		return cv::MorphTypes::MORPH_OPEN;
	}
	else if (name == "TOPHAT")
	{
		return cv::MorphTypes::MORPH_TOPHAT;
	}
	else
	{
		throw HipeException("Error in Erode::convertMorphType - Unknown morph type argument: " + name + ".\nMorphType name must not conatain the \"MORPH_\" prefix (i.e. ERODE for MORPH_ERODE).");
	}
}

int filter::algos::Erode::convertMorphShape(const std::string & name)
{
	if (name == "CROSS")
	{
		return cv::MorphShapes::MORPH_CROSS;
	}
	else if (name == "ELLIPSE")
	{
		return cv::MorphShapes::MORPH_ELLIPSE;
	}
	else if (name == "RECT")
	{
		return cv::MorphShapes::MORPH_RECT;
	}
	else
	{
		throw HipeException("Error in Erode::convertMorphShape - Unknown morph shape argument: " + name + ".\nMorphShape name must not conatain the \"MORPH_\" prefix (i.e. RECT for MORPH_RECT).");
	}
}
