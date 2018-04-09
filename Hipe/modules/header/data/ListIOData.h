#pragma once

#pragma warning(push, 0) 
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#pragma warning(pop)


#include <coredata/IODataType.h>
#include <coredata/IOData.h>

#include <coredata/data_export.h>

namespace data
{
	/**
	 * \brief ListIOData is the data type to use when you have multiple types of data to handle at the same time
	 */
	class DATA_EXPORT ListIOData : public IOData<Data, ListIOData>
	{
	protected:
		/**
		 * \brief Container of all the data
		 */
		std::vector<Data> _listIoData;

		ListIOData() : IOData(IODataType::LISTIO)
		{

		}

	public:
		/**
		 * \brief Constructor with a list of multiple \see Data objets.
		 * \param listIoData The \see Data. The multiple objects in the list can be of different types
		 */
		ListIOData(std::vector<Data> listIoData) : IOData(IODataType::LISTIO)
		{
			Data::registerInstance(new ListIOData());
			This()._listIoData = listIoData;
		}

		/**
		 * \brief Get the data
		 * \return Returns the data in a std::vector container
		 */
		std::vector<Data> getListIoData();

		ListIOData(const ListIOData& left) : IOData(left)
		{
			This()._listIoData = left._listIoData;
		}

		/**
		* \brief Copy the data of the ListIOData object to another one
		* \param left The other object where to copy the data to
		*/
		void copyTo(ListIOData& left) const;

		/**
		 * \brief Add data coming from another ListIOData object
		 * \param left The other object to copy the data from
		 * \param copy unused
		 */
		void Add(const ListIOData& left, bool copy = false);

		/**
		 * \brief ListIOData assignment operator
		 * \param left The other object to copy the data from
		 * \return A reference to the object
		 */
		virtual ListIOData& operator=(const ListIOData& left);

		/**
		* \brief Add data coming from another ListIOData object
		* \param left The other object to copy the data from
		*/
		virtual IOData& operator<<(const ListIOData& left);

		/**
		 * \brief
		 * \return Returns true if the object doesn't contain any data
		 */
		bool empty() const;

		/**
		 * \brief Get the object's data container (const version)
		 * \return Returns a constant reference to the object's data container
		 */
		const std::vector<Data>& getListData() const;

		/**
		 * \brief Overwrites the data of the object with the content of a container
		 * \param left The container to copy the data from
		 */
		void setListData(const std::vector<Data>& left);
	};
}
