#pragma once
#include <filter/tools/RegisterClass.h>
#include <core/HipeException.h>
#include <filter/IFilter.h>
#include <core/HipeStatus.h>
#include <data/ImageData.h>
#include <data/ShapeData.h>


//namespace filter
//{
//	namespace algos
//	{
//		/**
//		 * \var OverlayFilter::ratio
//		 * [TODO]
//		 */
//
//		/**
//		 * \todo
//		 * \brief The OverlayFilter filter is used contour regions of interests in an image.
//		 * 
//		 * The ConnexData port must contain 2 objects. The image on which the filter will draw, and the list of regions of interest (SquareCrop object).
//		 */
//		class OverlayFilter : public filter::IFilter
//		{
//			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
//			CONNECTOR(data::Data, data::ImageData);
//
//			REGISTER(OverlayFilter, ()), _connexData(data::INDATA)
//			{
//
//			}
//
//			REGISTER_P(double, ratio);
//
//			virtual std::string resultAsString() { return std::string("TODO"); };
//
//		public:
//			HipeStatus process()
//			{
//				if (_connexData.size() % 2 != 0)
//				{
//					throw HipeException("The Overlay missing or text data. Please be sure to link properly with parent");
//				}
//
//				//while (!_connexData.empty()) // While i've parent data
//				{
//					data::Data data1 = _connexData.pop();
//					data::Data data2 = _connexData.pop();
//
//					if (data1.getType() != data::IMGF &&
//						data1.getType() != data::TXT_ARR &&
//						data1.getType() != data::TXT &&
//						data1.getType() != data::SQR_CROP)
//					{
//						throw HipeException("The Overlay object cant aggregate tan text ATM. Please Develop OverlayFilter");
//					}
//					if (data2.getType() != data::IMGF &&
//						data2.getType() != data::TXT_ARR &&
//						data2.getType() != data::TXT &&
//						data2.getType() != data::SQR_CROP)
//					{
//						throw HipeException("The Overlay object cant aggregate tan text ATM. Please Develop OverlayFilter");
//					}
//					data::ImageData image;
//					if (data1.getType() != data::IMGF && data2.getType() != data::IMGF)
//					{
//						throw HipeException("Missing image to generate overlay text");
//					}
//					if (data1.getType() == data::IMGF)
//						image = static_cast<data::ImageData &>(data1);
//					if (data2.getType() == data::IMGF)
//						image = static_cast<data::ImageData &>(data2);
//
//					if (data1.getType() == data::TXT || data2.getType() == data::TXT_ARR)
//						throw HipeException("Text overlay is not yet implemented");
//
//					data::SquareCrop crops;
//					if (data1.getType() == data::SQR_CROP)
//						crops = static_cast<data::SquareCrop &>(data1);
//					if (data2.getType() == data::SQR_CROP)
//						crops = static_cast<data::SquareCrop &>(data2);
//
//					for (cv::Rect & rect : crops.getSquareCrop())
//					{
//						cv::rectangle(image.getMat(), rect, cv::Scalar(255, 0, 0));
//					}
//					_connexData.push(image);
//				}
//				return OK;
//			}
//		};
//
//		ADD_CLASS(OverlayFilter, ratio);
//	}
//}

namespace filter
{
	namespace algos
	{
		/**
		* \var OverlayFilter::ratio
		* [TODO]
		*/

		/**
		* \todo
		* \brief The OverlayFilter filter is used contour regions of interests in an image.
		*
		* The ConnexData port must contain 2 objects. The image on which the filter will draw, and the list of regions of interest (ShapeData object), or text.
		*/
		class OverlayFilter : public filter::IFilter
		{
			//data::ConnexData<data::ImageArrayData, data::ImageArrayData> _connexData;
			CONNECTOR(data::Data, data::ImageData);

			REGISTER(OverlayFilter, ()), _connexData(data::INDATA)
			{

			}

			REGISTER_P(double, ratio);

			virtual std::string resultAsString() { return std::string("TODO"); };

		public:
			HipeStatus process()
			{
				if (_connexData.size() % 2 != 0)
				{
					throw HipeException("The Overlay missing or text data. Please be sure to link properly with parent");
				}

				//while (!_connexData.empty()) // While i've parent data
				{
					data::Data data1 = _connexData.pop();
					data::Data data2 = _connexData.pop();
					data::ImageData image;
					data::ShapeData shapes;

					// Assert input types are correct
					if (!assertDataType(data1) || !assertDataType(data2))
					{
						throw HipeException("Error in OverlayFilter: The input types mismatch the required ones.");
					}

					if (data1.getType() != data::IMGF && data2.getType() != data::IMGF)
					{
						throw HipeException("Error in OverlayFilter: Missing image to generate overlay.");
					}

					// Find image
					if (data1.getType() == data::IMGF)
						image = static_cast<data::ImageData &>(data1);
					else if (data2.getType() == data::IMGF)
						image = static_cast<data::ImageData &>(data2);
					else if (data1.getType() == data::IMGF && data2.getType() == data::IMGF)
						throw HipeException("Error in OverlayFilter: Overlay filter doesn't accept two images in input. Use OverlayMat filter.");
					else
						throw HipeException("Error in OverlayFilter: Missing image to generate overlay.");

					// Find overlay data
					if (data1.getType() == data::SHAPE)
						shapes = static_cast<data::ShapeData &>(data1);
					else if (data2.getType() == data::SHAPE)
						shapes = static_cast<data::ShapeData &>(data2);
					else
						throw HipeException("Error in OverlayFilter: Missing shape data to generate overlay.");

					cv::Mat output = image.getMat().clone();

					const cv::Scalar pointsColor(0, 255, 255);
					const cv::Scalar circlesColor(0, 255, 0);
					const cv::Scalar rectsColor(255, 0, 0);
					const cv::Scalar quadsColor(255, 0, 255);

					// Draw points
					for (const cv::Point2f& point : shapes.PointsArray_const())
					{
						cv::circle(output, point, 2, pointsColor, 2);
					}

					// Draw circles
					for (const cv::Vec3f& circle : shapes.CirclesArray_const())
					{
						cv::Point2f center(circle[0], circle[1]);
						const float radius = circle[2];
						cv::circle(output, center, radius, circlesColor, 2);
					}

					// Draw rects
					for (const cv::Rect& rect : shapes.RectsArray_const())
					{
						cv::rectangle(output, rect, rectsColor, 2);
					}

					// Draw quads
					for (const data::four_points& quad : shapes.QuadrilatereArray_const())
					{
						for (size_t i = 0; i < quad.size() - 1; ++i)
						{
							cv::line(output, quad[i], quad[i + 1], quadsColor, 2);
						}
						cv::line(output, quad.front(), quad.back(), quadsColor, 2);
					}


					_connexData.push(data::ImageData(output));
				}
				return OK;
			}

			bool assertDataType(const data::Data& data)
			{
				switch (data.getType())
				{
				case data::IMGF:
				case data::SHAPE:
					return true;
				case data::TXT_ARR:
				case data::TXT:
				{
					std::stringstream errorMessage;
					errorMessage << "Error in OverlayFilter: The overlay filter can't display text ATM.\n";
					errorMessage << "\t found type: " << data::DataTypeMapper::getStringFromType(data.getType());
					throw HipeException(errorMessage.str());
				}
				default:
					return false;
				}
			}
		};



		ADD_CLASS(OverlayFilter, ratio);
	}
}
