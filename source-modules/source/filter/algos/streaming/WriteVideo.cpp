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

#include <algos/streaming/WriteVideo.h>
#include "data/FileVideoInput.h"
#include <regex>

int filter::algos::WriteVideo::getLastKnownIDFromDirectory()
{
	std::vector<cv::String> filenames;

	cv::glob(dir_path, filenames);

	std::stringstream file_pattern;
	file_pattern << prefix_filename << "_([0-9]+)\\.avi";

	std::regex base_regex(file_pattern.str());
	std::smatch base_match;
	int id = -1;
	for (const auto &fname : filenames) {
		std::string std_filename = extractFileName(fname) + ".avi";

		if (std::regex_match(std_filename, base_match, base_regex)) {
			// The first sub_match is the whole string; the next
			// sub_match is the first parenthesized expression.
			if (base_match.size() == 2) {
				std::ssub_match base_sub_match = base_match[1];
				std::string base = base_sub_match.str();
				std::cout << fname << " has an id of " << base << '\n';
				std::string::size_type sz;   // alias of size_t

				int i_dec = std::stoi(base, &sz);

				id = std::max(id, i_dec);
			}
		}
	}
	std::cout << " The best next ID is " << id + 1 << '\n';
	return id + 1;
}

HipeStatus filter::algos::WriteVideo::process()
{
	data::ImageData image_data = _connexData.pop();
	cv::Size size = image_data.getMat().size();

	if (skip_frame <= 0 || count % skip_frame == 0)
	{
		if (!writer.isOpened()) {
			if (id == -1)
			{
				id = getLastKnownIDFromDirectory();
			}
			std::stringstream build_path;
			build_path << dir_path << PathSeparator() << prefix_filename << "_" << id << ".avi";

		
			std::cout << "Video will be written into : [ " << build_path.str() << " ]" << std::endl;
			// Declare FourCC code
			int fourcc = CV_FOURCC('M', 'J', 'P', 'G');
			std::stringstream build_pipeline;
			build_pipeline << "appsrc !videoconvert !x264enc ! mp4mux ! filesink location='";
			build_pipeline << build_path.str() << "'";
			
			if (! url.empty())
			{
				build_pipeline.clear();
				build_pipeline << url;
			}
			
			writer.open(build_path.str(), 0, (double)fps_avg, size, true);
		}
		cv::Mat copy;
		if (!image_data.getMat().empty())
		{
			image_data.getMat().copyTo(copy);
			//cv::cvtColor(image_data.getMat(), copy, CV_BGR2YUV_I420);
			writer << copy;
		}
	}
	return OK;
}
