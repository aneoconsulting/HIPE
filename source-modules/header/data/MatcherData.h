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

#pragma once
#include <coredata/IOData.h>
#include <vector>

#pragma warning(push, 0) 
#include <opencv2/core/mat.hpp>
#pragma warning(pop) 

namespace data
{
	/**
	* \brief PointData is the data type used to handle multiple points. Uses OpenCV.
	*/
	class DATA_EXPORT MatcherData : public IOData <Data, MatcherData>
	{
	protected:
		cv::Mat _patternImage;					//<! [TODO] The pattern image to find on the request image.
		std::vector<cv::KeyPoint> _inliers1;		//<! The inliers1_const (the pertinent information) computed from the patternImage.

		cv::Mat _requestImage;					//<! [TODO] The request image on which we would like to find the patternImage.
		std::vector<cv::KeyPoint> _inliers2;		//<! The inliers2_const (the pertinent information) computed from the request image.

		std::vector<cv::DMatch> _goodMatches;	//<! The inliers1 and inliers2 that match (i.e. they were found on the patternImage and the requestImage).

		bool isBest;

	public:
		bool IsBest() const
		{
			const MatcherData & this_const = This_const();
			return this_const.isBest;
		}

		void setBest()
		{
			MatcherData &ret = This();
			isBest = true;
			ret.isBest = true;
		}

	protected:
		MatcherData(IOData::_Protection priv) : IOData(MATCHER)
		{
			isBest = false;
		}

		MatcherData(data::IODataType type) : IOData(type)
		{
			
			isBest = false;
		}

	public:

		/**
		* \brief ShapeData default constructor, the internal IODataType data type will be "SHAPE"
		*/
		MatcherData() : IOData(MATCHER)
		{
			Data::registerInstance(new MatcherData(_Protection()));
			This()._type = MATCHER;
			This().isBest = false;
		}


		/**
		* \brief ShapeData copy constructor
		* \param right The ShapeData to copy data from
		*/
		MatcherData(const data::MatcherData &right) : IOData(right._type)
		{
			Data::registerInstance(right._This);
			_decorate = right._decorate;
			_type = right.This_const()._type;
			
			
		}

		MatcherData(const cv::Mat& patternImage, const cv::Mat& requestImage, const std::vector<cv::KeyPoint>& patternInliers, const std::vector<cv::KeyPoint>& requestInliers, const std::vector<cv::DMatch>& goodMatches)
		{
			Data::registerInstance(new MatcherData(IOData::_Protection()));
			This()._type = IODataType::MATCHER;

			This()._patternImage = patternImage;
			This()._requestImage = requestImage;

			This()._inliers1 = patternInliers;
			This()._inliers2 = requestInliers;

			This()._goodMatches = goodMatches;

			This().isBest = false;
		}

		virtual ~MatcherData()
		{
			IOData::release();
			_patternImage.release();
			_goodMatches.clear();
			_inliers1.clear();
			_inliers2.clear();
			_requestImage.release();


			if (_This)
			{
				This()._patternImage.release();
				This()._goodMatches.clear();
				This()._inliers1.clear();
				This()._inliers2.clear();
				This()._requestImage.release();
			}
		}



		std::vector<cv::DMatch> & goodMatches();

		std::vector<cv::KeyPoint>& inliers1();

		std::vector<cv::KeyPoint>& inliers2();

		cv::Mat& patternImage();
		cv::Mat& requestImage();


		const std::vector<cv::DMatch> & goodMatches_const() const;

		const std::vector<cv::KeyPoint>& inliers1_const() const;

		const std::vector<cv::KeyPoint>& inliers2_const() const;

		const cv::Mat& patternImage_const() const;
		const cv::Mat& requestImage_const() const;

		/**
		 * \brief Set the good match array of the object
		 */
		void setGoodMatches(const std::vector<cv::DMatch>& goodMatches);
		/**
		 * \brief Set the inliers1 (pattern image inliers2_const) of the object
		 */
		void setInliers1(const std::vector<cv::KeyPoint>& patternImageInliers);
		/**
		 * \brief Set the inliers2 (request image inliers2_const) of the object
		 */
		void setInliers2(const std::vector<cv::KeyPoint>& requestImageInliers);
		/**
		 * \brief Set the pattern image of the object
		 */
		void setPatternImage(const cv::Mat& patternImage);
		/**
		 * \brief Set the request image of the object
		 */
		void setRequestImage(const cv::Mat& requestImage);


		/*MatcherData& operator<<(MatcherData md)
		{
			This()._inliers1.push_back(md.inliers1());
			return *this;
		}*/



		virtual void copyTo(MatcherData& left) const;

		/**
		* \brief
		* \return Returns true if the object doesn't contain any data
		*/
		inline bool empty() const override;

		/**
		* \todo
		* \brief ShapeData assignment operator
		* \param left The ShapeData oject to get the data from
		* \return A reference to the object
		*/
		//MatcherData& operator=(const MatcherData& left);
	};
}
