#pragma once

#include <boost/property_tree/ptree.hpp>

#include <core/HipeException.h>

#include <filter/data/IOData.h>
#include <filter/data/IODataType.h>

#include <filter/data/FileImageData.h>
#include <filter/data/FileVideoInput.h>
#include <filter/data/StreamVideoInput.h>
#include <filter/data/DirectoryImgData.h>
#include <filter/data/ListIOData.h>
#include <filter/data/SquareCrop.h>
#include <filter/data/PatternData.h>

namespace filter
{
	namespace data
	{
		std::shared_ptr<ListIOData> ret;

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
			static std::vector<T> as_vector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
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
			static inline bool checkJsonFieldExist(const boost::property_tree::ptree& jsonNode, std::string key, bool throwException = true)
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
			static Data loadImageFromFile(std::string strPath)
			{
				return static_cast<Data>(FileImageData(strPath));
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
			static Data loadImageFromRawData(const std::string & rawData, const std::string & format, int width, int height, int channels)
			{
				return static_cast<Data>(FileImageData(rawData, format, width, height, channels));
			}

			/**
			 * [TODO]
			 * \brief Wrapper function to load multiples image, from the path to their directory, as a DirectoryImgData object.
			 * \param strPath The images' directory's path
			 * \return the loaded images in a DirectoryImgData object (casted to the type Data)
			 */
			static Data loadImagesFromDirectory(std::string strPath)
			{
				return static_cast<Data>(DirectoryImgData(strPath));
			}



			/**
			 * [TODO]
			 * \brief Wrapper function to load a video, from its path extracted from its json node, as a FileVideoInput object.
			 * \param dataNode The data node from the json request tree containing the video to load
			 * \return The loaded video in a FileVideoInput object (casted to the type Data)
			 */
			static Data loadVideoFromFile(const boost::property_tree::ptree& dataNode)
			{
				std::string path = dataNode.get<std::string>("path");
				bool loop = false;
				if (dataNode.count("loop") != 0)
				{
					loop = dataNode.get<bool>("loop");
				}
				return static_cast<Data>(FileVideoInput(path, loop));
			}

			/**
			 * [TODO]
			 * \brief Wrapper function to open a stream, from its uri, as a StreamVideoInput object
			 * \param path the uri to the stream
			 * \return The opened stream in a StreamVideoInput object (casted to the type Data)
			 */
			static Data loadVideoFromStream(const std::string & path)
			{
				return static_cast<Data>(StreamVideoInput(path));
			}

			/**
			 * [TODO]
			 * \brief Wrapper function to load a list of data (LISTIO) as a ListIOData object
			 * \param the data node from the json request tree to query containing all the data
			 * \return the loaded data in a ListIOData object (casted to the type Data)
			 */
			static Data loadListIoData(const boost::property_tree::ptree& dataNode)
			{
				std::vector<Data> res;

				auto child = dataNode.get_child("array");
				for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
				{
					auto iodata = getDataFromComposer(itarray->second);
					res.push_back(iodata);
				}

				return static_cast<Data>(ListIOData(res));

			}

			/**
			 * [TODO]
			 * \brief Wrapper function to load the data from a pattern (PATTERN) as a PatternData object
			 * \param dataNode The data node from the json request tree to query containing all the data
			 * \return the loaded data in a PatternData oject (casted to the type Data)
			 */
			static Data loadPatternData(const boost::property_tree::ptree& dataNode)
			{
				std::vector<Data> res;
				auto child = dataNode.get_child("desc");
				for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
				{
					const std::string dataType = itarray->first;

					auto data = getDataFromComposer(dataType, itarray->second);

					res.push_back(data);
				}
				data::PatternData pattern(res);

				return static_cast<Data>(pattern);
			}

			/**
			 * [TODO]
			 * \brief Wrapper function to load the data from a [TODO] as a SquareCrop object
			 * \param cropTree The data note from the json request tree to query containing all the data
			 * \return the loaded data in a SquareCrop object (casted to the type Data)
			 */
			static Data loadSquareCrop(const boost::property_tree::ptree& cropTree)
			{
				std::vector<Data> res;
				std::vector<int> pts;

				auto pcitureJson = cropTree.get_child("IMGF");
				Data data_from_composer = getDataFromComposer("IMGF", pcitureJson);
				ImageData picture(static_cast<const ImageData &>(data_from_composer));


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
			static Data getDataFromComposer(const std::string datatype, const boost::property_tree::ptree& dataNode)
			{
				IODataType ioDataType = DataTypeMapper::getTypeFromString(datatype);
				switch (ioDataType)
				{
				case IODataType::IMGF:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadImageFromFile(dataNode.get<std::string>("path"));
				case IODataType::VIDF:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadVideoFromFile(dataNode);
				case IODataType::SEQIMGD:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadImagesFromDirectory(dataNode.get<std::string>("path"));
				case IODataType::STRMVID:
					filter::data::Composer::checkJsonFieldExist(dataNode, "path");
					return loadVideoFromStream(dataNode.get<std::string>("path"));
				case IODataType::LISTIO:
					filter::data::Composer::checkJsonFieldExist(dataNode, "array");
					return loadListIoData(dataNode);
				case IODataType::PATTERN:
					filter::data::Composer::checkJsonFieldExist(dataNode, "desc");
					return loadPatternData(dataNode);
				case IODataType::SQR_CROP:
					filter::data::Composer::checkJsonFieldExist(dataNode, "IMGF");
					return loadSquareCrop(dataNode);
				case IODataType::IMGB64:
				{
					filter::data::Composer::checkJsonFieldExist(dataNode, "data");
					filter::data::Composer::checkJsonFieldExist(dataNode, "format");
					std::string format = dataNode.get<std::string>("format");
					std::transform(format.begin(), format.end(), format.begin(), ::toupper);
					
					// width, height, and channels are stored in encoded data
					if (!(format == "JPG" || format == "PNG"))
					{
						filter::data::Composer::checkJsonFieldExist(dataNode, "channels");
						filter::data::Composer::checkJsonFieldExist(dataNode, "width");
						filter::data::Composer::checkJsonFieldExist(dataNode, "height");

						return loadImageFromRawData(dataNode.get<std::string>("data"), dataNode.get<std::string>("format"), dataNode.get<int>("width"), dataNode.get<int>("height"), dataNode.get<int>("channels"));
					}
					return loadImageFromRawData(dataNode.get<std::string>("data"), dataNode.get<std::string>("format"), 0, 0, 0);
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
			static Data getDataFromComposer(const boost::property_tree::ptree& dataNode)
			{
				filter::data::Composer::checkJsonFieldExist(dataNode, "type");
				auto datatype = dataNode.get<std::string>("type");

				return getDataFromComposer(datatype, dataNode);
			}
		};
	}
}
