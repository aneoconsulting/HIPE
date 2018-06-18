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
