#include <algos/streaming/AutoCloudOverflowGET.h>
#include "data/FileVideoInput.h"
#include <regex>

int filter::algos::AutoCloudOverflowGET::getLastKnownIDFromDirectory()
{
	std::vector<cv::String> filenames;

	cv::glob(dir_path, filenames);

	std::stringstream file_pattern;
	file_pattern << prefix_filename << "_([0-9]+)\\.JPEG";

	std::regex base_regex(file_pattern.str());
	std::smatch base_match;
	int id = -1;
	for (const auto &fname : filenames) {
		std::string std_filename = extractFileName(fname) + ".JPEG";

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

HipeStatus filter::algos::AutoCloudOverflowGET::savePaternPicture(const cv::Mat& picture)
{
	createDirectory(dir_path);
	std::stringstream build_path;

	if (id == -1)
	{
		id = getLastKnownIDFromDirectory();
	}

	build_path << dir_path << PathSeparator() << prefix_filename << "_" << id << ".JPEG";

	cv::imwrite(build_path.str(), picture);
	id++;
	return OK;
}



HipeStatus filter::algos::AutoCloudOverflowGET::process()
{
	data::ImageData data = _connexData.pop();






	return OK;
}
