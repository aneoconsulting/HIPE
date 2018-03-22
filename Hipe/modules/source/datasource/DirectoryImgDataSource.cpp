#include <filter/datasource/DirectoryImgDataSource.h>
data::ImageArrayData loadImagesData(std::string _directoryPath)
{
	std::vector<cv::String> filenames;
	data::ImageArrayData images;
	cv::glob(_directoryPath, filenames);

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
		images.Array().push_back(mat);
	}
	if (images.empty())
	{
		std::stringstream iss;
		iss << "No file loaded from directory : " << _directoryPath;
		throw HipeException(iss.str());
	}
	return images;
}

HipeStatus filter::datasource::DirectoryImgDataSource::process()
{
	if (!atomic_state.exchange(true))
	{
		data::ImageArrayData images;
		images = loadImagesData(directoryPath);
		_connexData.push(images);
		return OK;
	}
	_connexData.push(data::ImageArrayData());
	return END_OF_STREAM;
}
