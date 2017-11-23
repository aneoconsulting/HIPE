#pragma once
#include <data/IOData.h>

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
		MatcherData(IOData::_Protection priv) : IOData(MATCHER)
		{

		}

		MatcherData(data::IODataType type) : IOData(type)
		{

		}

	public:

		/**
		* \brief ShapeData default constructor, the internal IODataType data type will be "SHAPE"
		*/
		MatcherData() : IOData(MATCHER)
		{
			Data::registerInstance(new MatcherData(_Protection()));
			This()._type = MATCHER;
		}


		/**
		* \brief ShapeData copy constructor
		* \param right The ShapeData to copy data from
		*/
		MatcherData(const data::MatcherData &right) : IOData(right._type)
		{
			Data::registerInstance(right._This);
			_type = right.This_const()._type;
			_decorate = right._decorate;
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
		MatcherData& operator=(const MatcherData& left);
	};
}