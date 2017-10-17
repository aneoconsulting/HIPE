#pragma once

#include <queue>
#include <map>
#include <opencv2/core/mat.hpp>
#include "core/HipeException.h"
#include "IOData.h"
#include <core/queue/ConcurrentQueue.h>
#include <data/IOData.h>
#include <data/DataPort.h>

#include <data/data_export.h>

namespace data
{
	/**
	 * \brief The enum direction to explain if the input data will modified (INOUT) or read only
	 */
	enum WayData
	{
		INDATA,		//<! The data is only used in input. No data will be outputed
		INOUT,		//<! The input is linked to the output and the retrived data from the port is only a reference. Working on the data will alter the output.
		OUTDATA,	//<! The data is only used as output. There's no input data.
		NOWAY		//<! There's no input nor output.
	};

	template <class MyClass>
	class CopyObject
	{
	public:
		static MyClass copy(MyClass & left)
		{
			MyClass & instance = left;
			//TODO For Opencv operator= isn't a copy specialize the copy to avoid the reference
			// and call left.copyTo(instance);

			return instance;
		}
	};

	/**
	 * \brief The non template class parent for ConnexData. This class
	 * is used to contains multiple connector with different type
	 *
	 */
	class DATA_EXPORT ConnexDataBase
	{
	protected:
		/**
		 * \brief The WayData field informs how the port should handle the data
		 * \see WayData
		 */
		WayData _way;

	public:
		ConnexDataBase(WayData way) : _way(way) {}

		/**
		 * \brief Get the method how the port handles the data
		 * \return Returns the WayData value
		 * \see WayData
		 */
		inline WayData getWay() const;

		/**
		 * \brief [TODO]
		 * \return [TODO]
		 */
		inline virtual ConnexDataBase& getCast();

		/**
		 * \brief [TODO]
		 * \param right
		 * \return
		 */
		inline virtual ConnexDataBase& operator<<(ConnexDataBase& right);

		/**
		 * \brief [TODO]
		 * \return
		 */
		virtual DataPortBase &getPort()
		{
			throw HipeException("Cannot down cast to get Port");
		}
	};

	/**
	 * \brief The connector is here to connect all data by DataPort.
	 * \tparam Din the Input data type to accept in the IFiler Object (comming from parents)
	 * \tparam Dout the output data type to push to the next dataPort (going t the childrens)
	 */
	template <class Din, class Dout>
	class ConnexData : public ConnexDataBase
	{
	public:

		/**
		 * \brief The Dataport object used to store the input data.
		 * \see DataPort
		 */
		DataPort port;

		/**
		 * \brief Get the input port's object.
		 * \return Returns a reference to the port
		 */
		inline virtual DataPortBase &getPort()
		{
			return port;
		}

		/**
		 * \brief [TODO]
		 */
		std::map<ConnexDataBase *, DataPort *> portOutput;

		/**
		 * \brief Default ConnexData constructor. The WayData will be set to INDATA.
		 */
		ConnexData() : ConnexDataBase(INDATA)
		{

		}

		/**
		 * \brief Constructor with a WayData paremeter. The way data is used to tell how the port should handle data.
		 * \param way The desired WayData value.
		 * \see WayData
		 */
		ConnexData(WayData way) : ConnexDataBase(way)
		{

		}
		/**
		 * \brief Check if the port contains data.
		 * \return Returns true if there's no data in the port.
		 */
		inline bool empty() { return port.empty(); }

		/**
		 * \brief Get the number of stored elements in the port
		 * \return Returns the number of stored elements in the port
		 */
		inline size_t size() { return port.size(); }

		/**
		 * \brief Get the port's referenced next data.
		 * \return Returns the port's referenced next data.
		 * \see pop()
		 */
		Din get()
		{
			Din in = port.pop();
			return in;
		}

		/**
		 * \brief Get the port's referenced next data. If the way is INOUT, the port will keep a reference to the returned data. In that case modifying the data will affect the one on the port.
		 * \return Returns the port's referenced next data.
		 */
		virtual Din pop()
		{
			if (!port.empty())
			{
				const Data & popped = port.pop();
				Din in = popped;

				if (_way == INOUT)
				{
					broacast(in);
				}
				return in;
			}

			throw HipeException("No more data to pop");
		}



		/**
		 * \brief [TODO]
		 * \tparam DoutBroadCast
		 * \param dataOutput
		 */
		template <class DoutBroadCast>
		void broacast(DoutBroadCast dataOutput)
		{

		}

		/**
		 * [TODO]
		 * \brief Broadcat data to the port's output.
		 * \param dataOutput The data to broadcast
		 */
		void broacast(Dout & dataOutput)
		{

			for (auto& childPair : portOutput)
			{
				ConnexDataBase* child = childPair.first;
				/**
				* We need to copy the data to avoid to write on image when other children can use it
				*/
				if (portOutput.size() > 1 && child->getWay() == INOUT)
				{
					Dout cpy = CopyObject<Dout>::copy(dataOutput);
					portOutput[child]->push(cpy);
				}
				else
				{
					portOutput[child]->push(dataOutput);
				}
			}
		}

		/**
		 * \brief Send data to the port. The port will establish a link with the next filter of the graph.
		 * \param dataOutput The data the port should reference
		 */
		void push(Dout dataOutput)
		{

			if (_way == INOUT)
			{
				throw HipeException("An inout data can't push new data, relation is ONE input to ONE output");
			}

			broacast(dataOutput);
		}

		/**
		 * \brief [TODO]
		 * \tparam indata
		 * \param matrix
		 */
		template<class indata>
		void push(indata matrix)
		{
			Dout caps(matrix);
			push(caps);
		}

		/**
		 * \brief Link DataPort together to create the data graph
		 * \tparam childDin the type of the input
		 * \tparam childDout  the type if the output
		 * \param connexOut the neighboor children where the data port is coming from
		 */
		template <class childDin, class childDout>
		void connectOutput(ConnexData<childDin, childDout> &connexOut)
		{
			//template output of object This must be identical to childIn
			portOutput[&connexOut] = (&(connexOut.port));
		}

		/**
		 * [TODO]
		 * \brief  Link DataPort together to create the data graph
		 * \tparam leftIn the type of the input
		 * \tparam leftOut the type of the output
		 * \param left the neighbor children where the data port is coming from
		 * \return A reference to the ConnexData object
		 */
		template<class leftIn, class leftOut>
		ConnexData<leftIn, leftOut> & operator<<(ConnexData<leftIn, leftOut> &left)
		{
			this->connectOutput(left);

			return left;
		}

		/**
		 * [TODO]
		 * \brief Link DataPort together to create the data graph
		 * \param left the neighbor children where the data port is coming from
		 * \return A reference to the ConnexData object
		 */
		ConnexData<Din, Dout> & operator<<(ConnexDataBase &left)
		{
			portOutput[&left] = static_cast<DataPort *>(&(left.getPort()));

			return *this;
		}

		/**
		 * \brief [TODO]
		 * \return
		 */
		inline virtual ConnexData<Din, Dout> & getCast()
		{
			return *this;
		}
	};


	/*template <class Din, class Dout>
	template <class DoutBroadCast>
	template <>
	void ConnexData<Din, Dout>::broacast<WayData::NOWAY>(DoutBroadCast dataOutput)
	{

	}*/

	/**
	 * \brief A derived class of of ConnexData. It's a specialization to restrict the object to an input connector
	 * there is only one port connexion
	 * \tparam Din the data type to send to children
	 */
	template <class Din>
	class ConnexInput : public ConnexData<Din, Din> // Hard the *FIRST* input data is contained in the outputPort
	{
	public:
		ConnexInput()
			: ConnexData<Din, Din>(INDATA)
		{

		}

		ConnexInput(Din * data, WayData & way)
			: ConnexData<Din, Din>(way)
		{

		}

		void push(Din & dataOutput)
		{

			ConnexData<Din, Din>::broacast(dataOutput);

		}

		template<class indata>
		void push(indata matrix)
		{
			Din caps(matrix);
			ConnexData<Din, Din>::push(caps);

		}

		//template <>
		void connectInput(ConnexData<Din, Din> &connexIn)
		{
			throw HipeException("Can't add a port there no parent uppper to this object");
		}

		void forwardData()
		{
			if (!ConnexData<Din, Din>::port.empty())
			{
				Din in = ConnexData<Din, Din>::port.pop();
				if (ConnexData<Din, Din>::_way == INOUT)
				{
					ConnexData<Din, Din>::broacast(in);
				}
				else
				{

				}
			}
			throw HipeException("No more data to pop");
		}
	};


	/**
	* \brief A derived class of of ConnexData. It's a specialization to restrict the object to an output connector
	* there is only one port connexion
	* \tparam Dout the data type to receive from parent
	*/
	template <class Dout>
	class ConnexOutput : public ConnexData<Dout, Dout> // Hard the *FIRST* input data is contained in the outputPort
	{
	public:
		ConnexOutput()
			: ConnexData<Dout, Dout>(INDATA)
		{

		}

		Dout pop()
		{
			if (!ConnexData<Dout, Dout>::port.empty())
			{
				Dout in = ConnexData<Dout, Dout>::pop();
				return in;
			}

			throw HipeException("No more data to pop");
		}

		//template <>
		void ConnectOutput(ConnexData<Dout, Dout> &connexIn)
		{
			throw HipeException("Can't add a port there no children down to this object");
		}
	};
}
