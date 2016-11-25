#pragma once
#include <opencv2/core/mat.hpp>

class FileInfo
{
private:
	std::string _separator;

	std::string _dirname;
	std::string _filename;
	std::string _ext;

public:
	FileInfo()
	{
		char sep = '/';

#ifdef WIN32
		sep = '\\';
#endif
		_separator = sep;
	}


	std::string fullPath()
	{
		std::string path = _dirname + _separator + _filename;
		
		if (!_ext.empty())
		{
			path += "." + _ext;
		}
		return path;
	}

	std::string getFilename()
	{
		std::string path = _dirname + _separator + _filename;

		if (!_ext.empty())
		{
			path += "." + _ext;
		}
		return path;
	}

	std::string getDirname()
	{
		std::string path = _dirname + _separator + _filename;

		if (!_ext.empty())
		{
			path += "." + _ext;
		}
		return path;
	}

	std::string addPrefix(const std::string & prefix)
	{
		std::string save = _filename;
		_filename = prefix + _filename;

		std::string newName = fullPath();
		_filename = save;

		return newName;
	}

	friend FileInfo getFileName(const std::string& s);


};


void sortCorners(std::vector<cv::Point2f>& corners);

cv::Mat getImage(cv::String filename, int ratio = 5);
FileInfo getFileName(const std::string& s);

void BrightnessAndContrastAuto(const cv::Mat &src, cv::Mat &dst, float clipHistPercent = 0);

cv::Mat FilterLine(cv::Mat & inputGray);