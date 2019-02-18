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
#include <coredata/IODataType.h>
#include <coredata/data_export.h>
#include <coredata/IOData.h>

namespace data {

	class IVideo
	{
	public:
		

		virtual void openFile() = 0;
		virtual void closeFile() = 0;

		virtual Data newFrame() = 0;
	};

	/**
	 * \brief VideoData is the base data type used to handle videos.
	 * \tparam Derived
	 */
	template <typename Derived>
	class VideoData : public IOData<Data, Derived>, public IVideo
	{
	public:
		using IOData<Data, Derived>::IOData;


		VideoData(IODataType dataType) : IOData<Data, Derived>::IOData(dataType)
		{

		}

		VideoData(const VideoData &data) : IOData<Data, Derived>::IOData(data)
		{

		}
		virtual void openFile() { };

		virtual void closeFile() { };

		virtual ~VideoData() {}

		void copyTo(VideoData& left) const
		{
			static_cast<const Derived&>(*this).copyTo(static_cast<Derived&>(left));
		}

		virtual Data newFrame()
		{
			return static_cast<Derived&>(*this).newFrame();
		}

		bool empty() const
		{
			return static_cast<const Derived&>(*this).empty();
		}
	};
}
