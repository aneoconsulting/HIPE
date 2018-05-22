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

#include <data/DirectoryImgData.h>

#pragma warning(push, 0) 
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "ImageData.h"
#pragma warning(pop) 

namespace data
{
	void DirectoryImgData::loadImagesData()
	{
		cv::glob(This()._directoryPath, filenames);

		for (size_t i = 0; i < filenames.size(); ++i)
		{
			cv::Mat mat = cv::imread(filenames[i]);

			if (mat.empty())
			{
				std::stringstream strbuild;
				strbuild << "Cannot open file : " << filenames[i];
				throw HipeException(strbuild.str());
			}

			cv::putText(mat,
			            removeDirectoryName(filenames[i]),
			            cv::Point(25, 25), // Coordinates
			            cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
			            1.0, // Scale. 2.0 = 2x bigger
			            cv::Scalar(255, 255, 255), // Color
			            1); // Anti-alias
			This()._array.push_back(mat);
		}
		if (This()._array.empty())
		{
			std::stringstream iss;
			iss << "No file loaded from directory : " << _directoryPath;
			throw HipeException(iss.str());
		}
	}

	void DirectoryImgData::refreshDirectory()
	{
		This().filenames.clear();
		cv::glob(This()._directoryPath, This().filenames);
		This()._idxFile = 0;
	}

	ImageData DirectoryImgData::nextImageFile()
	{
		if (This()._idxFile >= This().filenames.size())
		{
			This()._idxFile = 0;
			return data::ImageData();
		}

		cv::Mat mat = cv::imread(This().filenames[This()._idxFile]);

		This()._idxFile++;

		return ImageData(mat);

	}

	std::vector<cv::Mat>& DirectoryImgData::images()
	{
		return This()._array;
	}

	cv::Mat DirectoryImgData::image(int index)
	{
		return This()._array[index];
	}
}
