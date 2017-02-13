#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include "IODataType.h"

namespace filter
{
	namespace data
	{
		class IOData
		{
		protected:
			std::vector<cv::Mat> _data;

			IODataType _type;

		public:
			IOData() : _type(IODataType::NONE)
			{
			}

			IOData(IODataType type) : _type(type)
			{
			}

		public:
			IOData(IOData& left) : IOData(left, false)
			{
			}

			IOData(const IOData& left, bool copy)
			{
				_type = left._type;
				_data.clear();

				for (auto& mat : left._data)
				{
					cv::Mat cur_mat;
					if (copy) mat.copyTo(cur_mat);
					else cur_mat = mat;

					_data.push_back(cur_mat);
				}
			}


			IOData(const IOData& left) : IOData(left, false)
			{
			}


		public:
			std::vector<cv::Mat>& getInputData()
			{
				return _data;
			}

			void setInputData(const std::vector<cv::Mat>& mats)
			{
				_data = mats;
			}

			cv::Mat& getInputData(int index)
			{
				return _data[index];
			}

			void addInputData(cv::Mat& mat)
			{
				_data.push_back(mat);
			}

			void copyTo(IOData& left)
			{
				IOData res(*this, true);

				left = res;
			}

			void copyRefTo(IOData& left)
			{
				IOData res(*this, false);

				left = res;
			}


			IOData& operator=(const IOData& left)
			{
				_type = left._type;
				_data.clear();

				for (auto& mat : left._data)
				{
					_data.push_back(mat);
				}
				return *this;
			}

			IOData& operator<<(const IOData& left)
			{
				if (_type != left._type) 
					throw HipeException("Cannot add data because types are different");

				for (auto& mat : left._data)
				{
					_data.push_back(mat);
				}
				return *this;
			}

			inline bool empty()
			{
				return _data.empty();
			}

			template<typename type>
			inline static type & downCast(data::IOData & outputData)
			{
				return static_cast<type &>(outputData);
			}


		};
	}
}
