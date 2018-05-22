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
#include <coredata/Data.h>
#include <coredata/IODataType.h>

#pragma warning(push, 0)   
#include <boost/thread/shared_mutex.hpp>
#include <dlib/image_processing.h>
#include <data/HogTrainer/HogTrainer.h>
#pragma warning(pop)

namespace data
{
	/**
	* \brief DlibDetectorData is the data class used to handle dlib detectors. Uses Dlib.
	*/
	class DATA_EXPORT DlibDetectorData : public data::IOData<data::Data, DlibDetectorData>
	{
		typedef dlib::object_detector<data::hog_trainer::image_scanner_type> detector_type;

	private:
		std::vector<detector_type> _detectors;

	protected:
		DlibDetectorData(IOData::_Protection priv) : IOData(data::IODataType::DLIBDTCT)
		{

		}

		DlibDetectorData(data::IODataType type) : IOData(type)
		{

		}

	public:
		/**
		* \brief DlibDetectorData default constructor, the internal IODataType data type will be "DLIBDTCT"
		*/
		DlibDetectorData() : IOData::IOData(data::IODataType::DLIBDTCT)
		{
			data::Data::registerInstance(new DlibDetectorData(IOData::_Protection()));
			This()._type = data::IODataType::DLIBDTCT;
			_type = data::IODataType::DLIBDTCT;
		}


		/**
		* \brief DlibDetectorData copy constructor
		* \param right The DlibDetectorData to copy data from
		*/
		DlibDetectorData(const data::DlibDetectorData& right) : IOData(right._type)
		{
			data::Data::registerInstance(right._This);

			_type = right.This_const()._type;
			_decorate = right._decorate;
			_detectors = right._detectors;
		}

		DlibDetectorData(const std::vector<detector_type> & detectors) : IOData(data::IODataType::DLIBDTCT)
		{
			data::Data::registerInstance(new DlibDetectorData(IOData::_Protection()));
			This()._type = data::IODataType::DLIBDTCT;
			This()._detectors = detectors;

			_type = data::IODataType::DLIBDTCT;
			_detectors = detectors;
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
		const std::vector<detector_type>& detectors_const() const;

		/**
		* \brief Accessor to the detectors container
		* \return Returns a reference to the detectors container.
		*/
		std::vector<detector_type>& detectors();

		/**
		* \brief DlibDetectorData assignment operator.
		* \param left The DlibDetectorData oject to get the data from.
		* \return A reference to the object.
		*/
		DlibDetectorData& operator=(const DlibDetectorData& left);

		void copyTo(DlibDetectorData& left) const;

		bool empty() const override;
	};
}
