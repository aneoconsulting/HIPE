#pragma once
#include <core/queue/ConcurrentQueue.h>

#include <data/Data.h>
#include <data/data_export.h>

namespace data
{
	/**
	* \todo
	* \brief
	*/
	class DataPortBase
	{

	};

	/**
	* \todo
	* \brief Data port contains the data to guarantee the transition with all Ifilter and IModel
	* \tparam D the Data type to transit between 2 and more connexData
	*/
	class DATA_EXPORT DataPort : public DataPortBase
	{
	public:
		core::queue::ConcurrentQueue<Data> data;	//<! The queue used to store the data

		/**
		* \brief Alias to the pop() method. Get the port's next stored data.
		* \see pop()
		* \return Returns the port's next stored data.
		*/
		Data get();

		/**
		* \brief Get the port's next stored data.
		* \return Returns the port's next stored data.
		*/
		Data pop();

		/**
		* \brief Make the port reference data. The port will establish a link to the next filter in the graph and make data exchange possible.
		* \param dataIn The data to reference in the port.
		*/
		void push(Data& dataIn);

		/**
		* \brief Check if there is data present in the port
		* \return Returns true if there is no data in the port
		*/
		bool empty();

		/**
		* \brief Get the number of stored elements in the port
		* \return Returns the number of stored elements in the port
		*/
		size_t size();
	};
}
