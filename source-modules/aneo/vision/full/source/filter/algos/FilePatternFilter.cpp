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

#include <filter/algos/FilePatternFilter.h>


namespace filter
{
	namespace algos
	{
		HipeStatus FilePatternFilter::process()
		{
			auto p = _connexData.pop();
			auto pathdir = static_cast<data::DirectoryImgData>(p.DirectoryImg()).DirectoryPath();
#ifdef WIN32
			auto fullPath = pathdir.append("\\").append(filePath); 
#else
			auto fullPath = pathdir.append("/").append(filePath); 
#endif
			data::ImageData imageInput = p.imageSource();
			cv::Mat imageLoaded = cv::imread(fullPath, CV_LOAD_IMAGE_ANYCOLOR);
			if(!imageLoaded.empty())
			{
				std::vector<int> crop = { 0,0, imageLoaded.size().width, imageLoaded.size().height };
				data::ImageData imagedata(imageLoaded);
				data::SquareCrop squarecrop;
				squarecrop << imagedata;
				squarecrop << crop;
				data::PatternData pattern(imageInput, squarecrop);
				PUSH_DATA(pattern);
			}
			else
			{
				return DATA_EMPTY;
			}
			
			return OK;
		}
	}
}
