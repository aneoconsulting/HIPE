#pragma once
#include <core/HipeException.h>
#include <data/IOData.h>
#include <data/IODataType.h>
#include <data/FileImageData.h>
#include <data/FileVideoInput.h>
#include <data/StreamVideoInput.h>
#include <data/DirectoryImgData.h>
#include <data/ListIOData.h>
#include <data/SquareCrop.h>
#include <data/PatternData.h>
#include <json/JsonTree.h>

namespace orchestrator
{
	/*std::shared_ptr<filter::data::ListIOData> ret;*/

	/**
	 *\todo
	 * \brief The composer class handles the extraction and loading of the data from the json graph
	 */
	class Composer
	{
	public:
		/**
		 * \brief [TODO]
		 * \tparam T
		 * \param pt
		 * \param key
		 * \return
		 */
		template <typename T>
		static std::vector<T> as_vector(const json::JsonTree & pt, const char* key)
		{
			std::vector<T> r;
			for (auto& item : pt.get_child(key))
				r.push_back(item.second.get_value<T>());
			return r;
		}

		//TMI
		/**
		 * \brief Checks if a json node contains a certain key
		 * \param jsonNode The node to query
		 * \param key The key to find
		 */
		static bool checkJsonFieldExist(const json::JsonTree& jsonNode, std::string key, bool throwException = true)
		{
			if (jsonNode.count(key) == 0)
			{
				if (throwException) throw HipeException("Cannot find field json request. Requested field is : " + key);

				return false;
			}

			return true;
		}

		/**
		 * [TODO]
		 * \brief Wrapper function to load an image, from its path, as a FileImageData object.
		 * \param strPath The path to the image
		 * \return the loaded image in a FileImage object (casted to the type Data)
		 */
		static filter::data::Data loadImageFromFile(std::string strPath)
		{
			return static_cast<filter::data::Data>(filter::data::FileImageData(strPath));
		}

		/**
		 * \todo
		 * [TODO]
		 * \brief Wrapper function to load an image, from its base64 raw data, as a FileImageData object.
		 * \param rawData the raw data of the image in base64 format. The first part must be a header containg information on the properties of the image
		 * \param compressed is the image compressed (like in jpg)?
		 * \param compression the format used to compress the image (like jpg)
		 * \return the loaded image in a FileImage object (casted to the type Data)
		 */
		static filter::data::Data loadImageFromRawData(const std::string & rawData, const std::string & format, int width, int height, int channels)
		{
			return static_cast<filter::data::Data>(filter::data::FileImageData(rawData, format, width, height, channels));
		}

		/**
		 * [TODO]
		 * \brief Wrapper function to load multiples image, from the path to their directory, as a DirectoryImgData object.
		 * \param strPath The images' directory's path
		 * \return the loaded images in a DirectoryImgData object (casted to the type Data)
		 */
		static filter::data::Data loadImagesFromDirectory(std::string strPath)
		{
			return static_cast<filter::data::Data>(filter::data::DirectoryImgData(strPath));
		}



		/**
		 * [TODO]
		 * \brief Wrapper function to load a video, from its path extracted from its json node, as a FileVideoInput object.
		 * \param dataNode The data node from the json request tree containing the video to load
		 * \return The loaded video in a FileVideoInput object (casted to the type Data)
		 */
		static filter::data::Data loadVideoFromFile(const json::JsonTree& dataNode)
		{
			std::string path = dataNode.get("path");
			bool loop = false;
			if (dataNode.count("loop") != 0)
			{
				loop = dataNode.getBool("loop");
			}
			return static_cast<filter::data::Data>(filter::data::FileVideoInput(path, loop));
		}

		/**
		 * [TODO]
		 * \brief Wrapper function to open a stream, from its uri, as a StreamVideoInput object
		 * \param path the uri to the stream
		 * \return The opened stream in a StreamVideoInput object (casted to the type Data)
		 */
		static filter::data::Data loadVideoFromStream(const std::string & path)
		{
			return static_cast<filter::data::Data>(filter::data::StreamVideoInput(path));
		}

		/**
		 * [TODO]
		 * \brief Wrapper function to load a list of data (LISTIO) as a ListIOData object
		 * \param the data node from the json request tree to query containing all the data
		 * \return the loaded data in a ListIOData object (casted to the type Data)
		 */
		static filter::data::Data loadListIoData(const json::JsonTree& dataNode);
		static bool checkIfDirectory(const std::map<std::basic_string<char>, json::JsonTree*>& map);
		/**
		 * [TODO]
		 * \brief Wrapper function to load the data from a pattern (PATTERN) as a PatternData object
		 * \param dataNode The data node from the json request tree to query containing all the data
		 * \return the loaded data in a PatternData oject (casted to the type Data)
		 */
		static filter::data::Data loadPatternData(const json::JsonTree& dataNode);
		/**
		 * [TODO]
		 * \brief Wrapper function to load the data from a [TODO] as a SquareCrop object
		 * \param cropTree The data note from the json request tree to query containing all the data
		 * \return the loaded data in a SquareCrop object (casted to the type Data)
		 */
		static filter::data::Data loadSquareCrop(const json::JsonTree& cropTree)
		{
			std::vector<filter::data::Data> res;
			std::vector<int> pts;

			auto pcitureJson = cropTree.get_child("IMGF");
			filter::data::Data data_from_composer = getDataFromComposer("IMGF", pcitureJson);
			filter::data::ImageData picture(static_cast<const filter::data::ImageData &>(data_from_composer));


			if (cropTree.count("crop") != 0)
			{
				pts = as_vector<int>(cropTree, "crop");
			}
			else
			{
				const cv::Mat & cropImage = picture.getMat();

				//Then the image itself is the crop
				pts.push_back(0); pts.push_back(0);
				pts.push_back(cropImage.cols); pts.push_back(cropImage.rows);
			}
			filter::data::SquareCrop squareCrop(picture, pts);

			return squareCrop;
		}


		/**
		 * [TODO]
		 * \brief Extract the data from a json tree node and load it to its corresponding type
		 * \param datatype the type of the data to extract and load
		 * \param dataNode The node containing the data
		 * \return the loaded data in its corresponding type (casted to the type Data)
		 */
		static filter::data::Data getDataFromComposer(const std::string datatype, const json::JsonTree& dataNode)
		{
			using namespace filter::data;
			filter::data::IODataType ioDataType = filter::data::DataTypeMapper::getTypeFromString(datatype);
			switch (ioDataType)
			{
			case filter::data::IODataType::IMGF:
				Composer::checkJsonFieldExist(dataNode, "path");
				return loadImageFromFile(dataNode.get("path"));
			case filter::data::IODataType::VIDF:
				Composer::checkJsonFieldExist(dataNode, "path");
				return loadVideoFromFile(dataNode);
			case IODataType::SEQIMGD:
				Composer::checkJsonFieldExist(dataNode, "path");
				return loadImagesFromDirectory(dataNode.get("path"));
			case IODataType::STRMVID:
				Composer::checkJsonFieldExist(dataNode, "path");
				return loadVideoFromStream(dataNode.get("path"));
			case IODataType::LISTIO:
				Composer::checkJsonFieldExist(dataNode, "array");
				return loadListIoData(dataNode);
			case IODataType::PATTERN:
				Composer::checkJsonFieldExist(dataNode, "desc");
				return loadPatternData(dataNode);
			case IODataType::SQR_CROP:
				Composer::checkJsonFieldExist(dataNode, "IMGF");
				return loadSquareCrop(dataNode);
			case IODataType::IMGB64:
			{
				Composer::checkJsonFieldExist(dataNode, "data");
				Composer::checkJsonFieldExist(dataNode, "format");
				std::string format = dataNode.get("format");
				std::transform(format.begin(), format.end(), format.begin(), ::toupper);

				// width, height, and channels are stored in encoded data
				if (!(format == "JPG" || format == "PNG"))
				{
					Composer::checkJsonFieldExist(dataNode, "channels");
					Composer::checkJsonFieldExist(dataNode, "width");
					Composer::checkJsonFieldExist(dataNode, "height");

					return loadImageFromRawData(dataNode.get("data"), dataNode.get("format"), dataNode.getInt("width"), dataNode.getInt("height"), dataNode.getInt("channels"));
				}
				return loadImageFromRawData(dataNode.get("data"), dataNode.get("format"), 0, 0, 0);
			}
			case IODataType::NONE:
			default:
				throw HipeException("Cannot found the data type requested");
			}
		}

		/**
		 * [TODO]
		 * \brief Extract the data from a json tree node (if existing) and load it to its corresponding type
		 * \param dataNode The node to query
		 * \return the loaded data (if existing) in its corresponding type (casted to the type Data)
		 */
		static filter::data::Data getDataFromComposer(const json::JsonTree& dataNode);

	};
}
