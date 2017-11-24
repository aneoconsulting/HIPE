#pragma once
#include <data/IOData.h>

#include <dlib/image_processing.h>

namespace filter
{
	namespace data
	{
		/**
		 * \brief DlibDetectorData is the data class used to handle dlib detectors. Uses Dlib.
		 */
		class DlibDetectorData : public IOData <Data, DlibDetectorData>
		{
			typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6> > image_scanner_type;
			typedef dlib::object_detector<image_scanner_type> detector_type;

		private:
			std::vector<detector_type> _detectors;

		protected:

			DlibDetectorData(IOData::_Protection priv) : IOData(DLIBDTCT)
			{

			}

			DlibDetectorData(data::IODataType type) : IOData(type)
			{

			}

		public:
			using IOData::IOData;


			/**
			 * \brief DlibDetectorData default constructor, the internal IODataType data type will be "DLIBDTCT"
			 */
			DlibDetectorData() : IOData(DLIBDTCT)
			{
				Data::registerInstance(new DlibDetectorData(IOData::_Protection()));
				This()._type = DLIBDTCT;
				_type = DLIBDTCT;
			}


			/**
			 * \brief DlibDetectorData copy constructor
			 * \param right The DlibDetectorData to copy data from
			 */
			DlibDetectorData(const data::DlibDetectorData &right) : IOData(right._type)
			{
				Data::registerInstance(right._This);
				_type = right.This_const()._type;
				_decorate = right._decorate;
			}

			DlibDetectorData(const std::vector<detector_type> & detectors) : IOData(DLIBDTCT)
			{
				Data::registerInstance(new DlibDetectorData(IOData::_Protection()));
				This()._type = DLIBDTCT;
				This()._detectors = detectors;

				_type = DLIBDTCT;
			}

			virtual ~DlibDetectorData()
			{
				IOData::release();

				_detectors.clear();

				if (_This)
				{
					This()._detectors.clear();
				}
			}



			/**
			 * \brief Accessor (const version) to the detectors container
			 * \return Returns a const reference to the detectors container.
			 */
			const std::vector<detector_type>& detectors_const() const
			{
				return This_const()._detectors;
			}

			/**
			 * \brief Accessor to the detectors container
			 * \return Returns a reference to the detectors container.
			 */
			std::vector<detector_type>& detectors()
			{
				return This()._detectors;
			}

			/**
			 * \brief Add detectors to the detectors container.
			 * \param detectors The detectors to add.
			 * \return Returns a reference to the DlibDetectorData object.
			 */
			DlibDetectorData& operator<<(const std::vector<detector_type>& detectors)
			{
				This()._detectors.insert(This()._detectors.end(), detectors.begin(), detectors.end());
				return *this;
			}

			/**
			 * \brief Add detector to the detectors container.
			 * \param detector The detector to add.
			 * \return Returns a reference to the DlibDetectorData object.
			 */
			DlibDetectorData& operator<<(const detector_type& detector)
			{
				This()._detectors.push_back(detector);
				return *this;
			}


			/**
			 * \brief Copy the data of the object to another one
			 * \param left The other object where to copy the data to. Its current data will not be overwritten
			 */
			virtual void copyTo(DlibDetectorData& left) const
			{
				if (left.getType() != getType())
					throw HipeException("ERROR - data::DlibDetectorData::copyTo - cannot copy object data. Types mismatch.");

				left.This()._detectors.insert(left.This()._detectors.end(), This_const()._detectors.begin(), This_const()._detectors.end());
			}

			/**
			 * \brief
			 * \return Returns true if the object doesn't contain any data
			 */
			inline bool empty() const override
			{
				return This_const()._detectors.empty();
			}

			/**
			 * \brief DlibDetectorData assignment operator.
			 * \param left The DlibDetectorData oject to get the data from.
			 * \return A reference to the object.
			 */
			DlibDetectorData& operator=(const DlibDetectorData& left)
			{
				Data::registerInstance(left);

				//_This = left._This;

				_type = left._type;
				_decorate = left._decorate;

				return *this;
			}
		};
	}
}
