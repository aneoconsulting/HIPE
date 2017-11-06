#pragma once
#include <data/IOData.h>

namespace filter
{
	namespace data
	{
		/**
		* \brief PointData is the data type used to handle multiple points. Uses OpenCV.
		*/
		class MatcherData : public IOData <Data, MatcherData>
		{
		protected:
			cv::Mat _patternImage;					//<! [TODO] The pattern image to find on the request image.
			std::vector<cv::KeyPoint> _inliers1;		//<! The inliers (the pertinent information) computed from the patternImage.

			cv::Mat _requestImage;					//<! [TODO] The request image on which we would like to find the patternImage.
			std::vector<cv::KeyPoint> _inliers2;		//<! The inliers (the pertinent information) computed from the request image.

			std::vector<cv::DMatch> _goodMatches;	//<! The inliers1 and inliers2 that match (i.e. they were found on the patternImage and the requestImage).
			MatcherData(IOData::_Protection priv) : IOData(MATCHER)
			{

			}

			MatcherData(data::IODataType type) : IOData(type)
			{

			}

		public:
			using IOData::IOData;


			/**
			* \brief ShapeData default constructor, the internal IODataType data type will be "SHAPE"
			*/
			MatcherData() : IOData(MATCHER)
			{
				Data::registerInstance(new MatcherData(IOData::_Protection()));
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


		
			std::vector<cv::DMatch> & GoodMatches()
			{
				MatcherData &ret = This();
				return ret._goodMatches;
			}

			std::vector<cv::KeyPoint>& Inliers1()
			{
				MatcherData &ret = This();
				return ret._inliers1;
			}

			std::vector<cv::KeyPoint>& Inliers2()
			{
				MatcherData &ret = This();
				return ret._inliers2;
			}

			cv::Mat& PatternImage()
			{
				MatcherData &ret = This();
				return ret._patternImage;
			}
			cv::Mat& RequestImage()
			{
				MatcherData &ret = This();
				return ret._requestImage;
			}

			
			const std::vector<cv::DMatch> & GoodMatches_const() const 
			{
				const MatcherData &ret = This_const();
				return ret._goodMatches;
			}

			const std::vector<cv::KeyPoint>& Inliers1_const() const
			{
				const MatcherData &ret = This_const();
				return ret._inliers1;
			}

			const std::vector<cv::KeyPoint>& Inliers2_const() const
			{
				const MatcherData &ret = This_const();
				return ret._inliers2;
			}

			const cv::Mat& PatternImage_const() const
			{
				const MatcherData &ret = This_const();
				return ret._patternImage;
			}
			const cv::Mat& RequestImage_const() const
			{
				const MatcherData &ret = This_const();
				return ret._requestImage;
			}


			
			/*MatcherData& operator<<(MatcherData md)
			{
				This()._inliers1.push_back(md.Inliers1());
				return *this;
			}*/

			
		
			virtual void copyTo(MatcherData& left) const
			{
				if (left.getType() != getType())
					throw HipeException("ERROR - data::MatcherData - cannot copty data. Types mismatch.");


				for (auto matches  : GoodMatches_const())
				{
					left.This()._goodMatches.push_back(matches);
				}

				for (auto inlier1 : Inliers1_const())
				{
					left.This()._inliers1.push_back(inlier1);
				}

				for (auto inlier2 : Inliers2_const())
				{
					left.This()._inliers2.push_back(inlier2);
				}
				for (auto inlier2 : Inliers2_const())
				{
					left.This()._inliers2.push_back(inlier2);
				}
				left.This()._patternImage.copyTo(PatternImage_const());
				left.This()._requestImage.copyTo(RequestImage_const());
			}

			/**
			* \brief
			* \return Returns true if the object doesn't contain any data
			*/
			inline bool empty() const override
			{
				return (This_const()._inliers1.empty() && This_const()._inliers2.empty() && This_const()._goodMatches.empty()&& This_const()._patternImage.empty() && This_const()._requestImage.empty());
			}

			/**
			* \todo
			* \brief ShapeData assignment operator
			* \param left The ShapeData oject to get the data from
			* \return A reference to the object
			*/
			MatcherData& operator=(const MatcherData& left)
			{
				_This = left._This;
				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}
		};
	}
}
