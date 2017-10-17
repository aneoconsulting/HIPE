#pragma once

#include <boost/property_tree/ptree.hpp>

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

#include <data/data_export.h>

namespace data
{
	std::shared_ptr<ListIOData> ret;

	/**
	 *\todo
	 * \brief The composer class handles the extraction and loading of the data from the json graph
	 */
	class DATA_EXPORT Composer
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
		static inline bool checkJsonFieldExist(const boost::property_tree::ptree& jsonNode, std::string key, bool throwException = true);

		/**
		 * [TODO]
		 * \brief Wrapper function to load an image, from its path, as a FileImageData object.
		 * \param strPath The path to the image
		 * \return the loaded image in a FileImage object (casted to the type Data)
		 */
		static inline Data loadImageFromFile(std::string strPath);

		/**
		 * \todo
		 * [TODO]
		 * \brief Wrapper function to load an image, from its base64 raw data, as a FileImageData object.
		 * \param rawData the raw data of the image in base64 format. The first part must be a header containg information on the properties of the image
		 * \param compressed is the image compressed (like in jpg)?
		 * \param compression the format used to compress the image (like jpg)
		 * \return the loaded image in a FileImage object (casted to the type Data)
		 */
		static inline Data loadImageFromRawData(const std::string& rawData, const std::string& format, int width, int height, int channels);

		/**
		 * [TODO]
		 * \brief Wrapper function to load multiples image, from the path to their directory, as a DirectoryImgData object.
		 * \param strPath The images' directory's path
		 * \return the loaded images in a DirectoryImgData object (casted to the type Data)
		 */
		static inline Data loadImagesFromDirectory(std::string strPath);

		/**
		 * [TODO]
		 * \brief Wrapper function to load a video, from its path extracted from its json node, as a FileVideoInput object.
		 * \param dataNode The data node from the json request tree containing the video to load
		 * \return The loaded video in a FileVideoInput object (casted to the type Data)
		 */
		static Data loadVideoFromFile(const boost::property_tree::ptree& dataNode);

		/**
		 * [TODO]
		 * \brief Wrapper function to open a stream, from its uri, as a StreamVideoInput object
		 * \param path the uri to the stream
		 * \return The opened stream in a StreamVideoInput object (casted to the type Data)
		 */
		static inline Data loadVideoFromStream(const std::string & path)
		{
			return static_cast<Data>(StreamVideoInput(path));
		}

		/**
		 * [TODO]
		 * \brief Wrapper function to load a list of data (LISTIO) as a ListIOData object
		 * \param the data node from the json request tree to query containing all the data
		 * \return the loaded data in a ListIOData object (casted to the type Data)
		 */
		static Data loadListIoData(const boost::property_tree::ptree& dataNode);
		/**
		 * [TODO]
		 * \brief Wrapper function to load the data from a pattern (PATTERN) as a PatternData object
		 * \param dataNode The data node from the json request tree to query containing all the data
		 * \return the loaded data in a PatternData oject (casted to the type Data)
		 */
		static Data loadPatternData(const boost::property_tree::ptree& dataNode);
		/**
		 * [TODO]
		 * \brief Wrapper function to load the data from a [TODO] as a SquareCrop object
		 * \param cropTree The data note from the json request tree to query containing all the data
		 * \return the loaded data in a SquareCrop object (casted to the type Data)
		 */
		static Data loadSquareCrop(const boost::property_tree::ptree& cropTree);

		/**
		 * [TODO]
		 * \brief Extract the data from a json tree node and load it to its corresponding type
		 * \param datatype the type of the data to extract and load
		 * \param dataNode The node containing the data
		 * \return the loaded data in its corresponding type (casted to the type Data)
		 */
		static Data getDataFromComposer(const std::string datatype, const boost::property_tree::ptree& dataNode);

		/**
		 * [TODO]
		 * \brief Extract the data from a json tree node (if existing) and load it to its corresponding type
		 * \param dataNode The node to query
		 * \return the loaded data (if existing) in its corresponding type (casted to the type Data)
		 */
		static Data getDataFromComposer(const boost::property_tree::ptree& dataNode);
	};
}
