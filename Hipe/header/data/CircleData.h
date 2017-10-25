#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <data/IODataType.h>
#include <data/IOData.h>

#include <data/data_export.h>

namespace data
{
	class DATA_EXPORT CircleData : public IOData<Data, CircleData>
	{
	protected:

		std::vector<cv::Vec3f> _circledata;

		CircleData() : IOData(IODataType::CIRCLE)
		{

		}

	public:
		
		CircleData(std::vector<cv::Vec3f> circledata) : IOData(IODataType::CIRCLE)
		{
			Data::registerInstance(new CircleData());
			This()._circledata = circledata;
		}

		
		inline std::vector<cv::Vec3f> getCircles();

		CircleData(const CircleData& left) : IOData(left)
		{
			This()._circledata = left._circledata;
		}

		void copyTo(CircleData& left) const;


		void Add(const CircleData& left, bool copy = false);

		virtual CircleData& operator=(const CircleData& left);

		virtual IOData& operator<<(const CircleData& left);


		inline bool empty() const;

		inline const std::vector<cv::Vec3f>& getCircles() const;

		/**
		* \brief Overwrites the data of the object with the content of a container
		* \param left The container to copy the data from
		*/
		inline void setCircles(const std::vector<cv::Vec3f>& left);
	};
}
