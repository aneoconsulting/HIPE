#pragma once


#pragma once

#include <opencv2/opencv.hpp>
#include <boost/filesystem/path.hpp>
#include <data/VideoData.h>

#ifdef OLD_CODE
#include <streaming/CaptureVideo.h>
#endif

#include <coredata/data_export.h>
#include <data/ImageData.h>


namespace data
{
	/**
	 * \brief Input filter used to acquire data from a stream
	 */
	class DATA_EXPORT StreamVideoInput : public VideoData<StreamVideoInput>
	{
		/**
		 * \brief the url (path) to the stream
		 */
		boost::filesystem::path _filePath;
		/**
		 * \brief the data of the stream
		 */
#ifdef OLD_CODE
		std::shared_ptr<CaptureVideo> _capture;
#endif

		cv::Mat asOutput() const;

	private:
		StreamVideoInput();

	public:
		using VideoData<StreamVideoInput>::VideoData;


		StreamVideoInput(const StreamVideoInput &data);

		/**
		 * \brief Constructor with stream's url as an std::string object
		 * \param url The url to the stream
		 */
		StreamVideoInput(const std::string & url);


		virtual ~StreamVideoInput();

		/**
		 * \brief
		 * \return Returns the stream's next frame or a black image if the playback ended
		 */
		Data newFrame();

		/**
		 * \brief
		 * \return Returns true if there's no data
		 */
		bool empty() const;
	};
}
