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

#include <coredata/IOData.h>
#include <json/JsonTree.h>
#include <Composer.h>
#include <coredata/NoneData.h>

#pragma warning(push, 0)
#include <boost/property_tree/ptree.hpp>
#pragma warning(pop)


namespace orchestrator
{
	
	bool Composer::checkJsonFieldExist(const json::JsonTree& jsonNode, std::string key, bool throwException)
	{
		if (jsonNode.count(key) == 0)
		{
			if (throwException) throw HipeException("Cannot find field json request. Requested field is : " + key);

			return false;
		}

		return true;
	}

	/*data::Data orchestrator::Composer::loadListIoData(const json::JsonTree& dataNode)
	{
		std::vector<data::Data> res;

		auto child = dataNode.allchildren("array");
		for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
		{
			auto iodata = getDataFromComposer(itarray->second);
			res.push_back(iodata);
		}

		return static_cast<data::Data>(data::ListIOData(res));
	}*/

	//data::Data Composer::loadImageFromFile(std::string strPath)
	//{
	//	return static_cast<data::Data>(data::FileImageData(strPath));
	//}

	//data::Data Composer::loadImageFromRawData(const std::string& rawData, const std::string& format, int width, int height, int channels)
	//{
	//	return static_cast<data::Data>(data::FileImageData(rawData, format, width, height, channels));
	//}

	//data::Data Composer::loadImagesFromDirectory(std::string strPath)
	//{
	//	data::DirectoryImgData directory_img_data = data::DirectoryImgData(strPath);
	//	directory_img_data.loadImagesData();

	//	return static_cast<data::Data>(directory_img_data);
	//}

	//data::Data Composer::loadVideoFromFile(const json::JsonTree& dataNode)
	//{
	//	std::string path = dataNode.get<std::string>("path");
	//	bool loop = false;
	//	if (dataNode.count("loop") != 0)
	//	{
	//		loop = dataNode.getBool("loop");
	//	}
	//	return static_cast<data::Data>(data::FileVideoInput(path, loop));
	//}

	//data::Data Composer::loadVideoFromStream(const std::string& path)
	//{
	//	return static_cast<data::Data>(data::StreamVideoInput(path));
	//}

	//data::Data Composer::loadPatternData(const json::JsonTree& dataNode)
	//{
	//	using namespace data;
	//	std::vector<Data> res;
	//	auto child = dataNode.allchildren("desc");
	//	bool isDirPAtterData = false;
	//	for (auto itarray = child.begin(); itarray != child.end(); ++itarray)
	//	{
	//		const std::string dataType = itarray->first;
	//		IODataType ioDataType = DataTypeMapper::getTypeFromString(dataType);
	//		if (ioDataType == SQR_CROP)
	//		{
	//			if (itarray->second.count("SEQIMGD") == 1) {
	//				isDirPAtterData = true;
	//				auto seqimgd = itarray->second.get_child("SEQIMGD");
	//				auto data = getDataFromComposer("SEQIMGD", seqimgd);
	//				res.push_back(data);
	//			}

	//			else if (itarray->second.count("IMGF") == 1)
	//			{
	//				auto inputData = itarray->second.get_child("IMGF");
	//				auto outputData = getDataFromComposer("IMGF", inputData);
	//				const cv::Mat & imageData = static_cast<const ImageData &>(outputData).getMat();

	//				// We still need a square crop object to create a Pattern Data
	//				cv::Rect imageRect(0, 0, imageData.cols, imageData.rows);
	//				std::vector<cv::Rect> rects;
	//				rects.push_back(imageRect);
	//				SquareCrop crop(imageData, rects);
	//				res.push_back(crop);
	//			}
	//		}
	//		else {
	//			auto data = getDataFromComposer(dataType, itarray->second);
	//			res.push_back(data);
	//		}

	//	}
	//	if (isDirPAtterData)
	//	{
	//		DirPatternData dirPattern(res);
	//		return static_cast<Data>(dirPattern);
	//	}
	//	PatternData pattern(res);

	//	return static_cast<Data>(pattern);
	//}

	//data::Data Composer::loadSquareCrop(const json::JsonTree& cropTree)
	//{
	//	std::vector<data::Data> res;
	//	std::vector<int> pts;

	//	auto pcitureJson = cropTree.get_child("IMGF");
	//	data::Data data_from_composer = getDataFromComposer("IMGF", pcitureJson);
	//	data::ImageData picture(static_cast<const data::ImageData &>(data_from_composer));


	//	if (cropTree.count("crop") != 0)
	//	{
	//		pts = as_vector<int>(cropTree, "crop");
	//	}
	//	else
	//	{
	//		const cv::Mat & cropImage = picture.getMat();

	//		//Then the image itself is the crop
	//		pts.push_back(0); pts.push_back(0);
	//		pts.push_back(cropImage.cols); pts.push_back(cropImage.rows);
	//	}
	//	data::SquareCrop squareCrop(picture, pts);

	//	return static_cast<data::Data>(squareCrop);
	//}
	//	
	//data::Data orchestrator::Composer::getDataFromComposer(const std::string datatype, const json::JsonTree& dataNode)
	//{
	//	data::IODataType ioDataType = data::DataTypeMapper::getTypeFromString(datatype);
	//	switch (ioDataType)
	//	{
	//	case data::IODataType::IMGF:
	//		Composer::checkJsonFieldExist(dataNode, "path");
	//		return loadImageFromFile(dataNode.get<std::string>("path"));
	//	case data::IODataType::VIDF:
	//		Composer::checkJsonFieldExist(dataNode, "path");
	//		return loadVideoFromFile(dataNode);
	//	case data::IODataType::SEQIMGD:
	//		Composer::checkJsonFieldExist(dataNode, "path");
	//		return loadImagesFromDirectory(dataNode.get<std::string>("path"));
	//	case data::IODataType::STRMVID:
	//		Composer::checkJsonFieldExist(dataNode, "path");
	//		return loadVideoFromStream(dataNode.get<std::string>("path"));
	//	case data::IODataType::LISTIO:
	//		Composer::checkJsonFieldExist(dataNode, "array");
	//		return loadListIoData(dataNode);
	//	case data::IODataType::PATTERN:
	//		Composer::checkJsonFieldExist(dataNode, "desc");
	//		return loadPatternData(dataNode);
	//	case data::IODataType::SQR_CROP:
	//		Composer::checkJsonFieldExist(dataNode, "IMGF");
	//		return loadSquareCrop(dataNode);
	//	case data::IODataType::IMGB64:
	//	{
	//		Composer::checkJsonFieldExist(dataNode, "data");
	//		Composer::checkJsonFieldExist(dataNode, "format");
	//		std::string format = dataNode.get("format");
	//		std::transform(format.begin(), format.end(), format.begin(), ::toupper);

	//		// width, height, and channels are stored in encoded data
	//		if (!(format == "JPG" || format == "PNG"))
	//		{
	//			Composer::checkJsonFieldExist(dataNode, "channels");
	//			Composer::checkJsonFieldExist(dataNode, "width");
	//			Composer::checkJsonFieldExist(dataNode, "height");

	//			return loadImageFromRawData(dataNode.get("data"), dataNode.get("format"), dataNode.getInt("width"), dataNode.getInt("height"), dataNode.getInt("channels"));
	//		}
	//		return loadImageFromRawData(dataNode.get("data"), dataNode.get("format"), 0, 0, 0);
	//	}
	//	case data::IODataType::NONE:
	//	default:
	//		throw HipeException("Cannot found the data type requested");
	//	}
	//}

	data::Data Composer::getDataFromComposer(const json::JsonTree& dataNode)
	{
		using namespace data;

		//Ignore the composer if there datasource inside data node. 
		// This is next form to load data
		if (dataNode.count("datasource") != 0)
			return NoneData();

		throw HipeException("There is no datasource in the json request. The data object should be in Datasource form");
		/*checkJsonFieldExist(dataNode, "type");
		auto datatype = dataNode.get("type");

		return getDataFromComposer(datatype, dataNode);*/
	}
}
