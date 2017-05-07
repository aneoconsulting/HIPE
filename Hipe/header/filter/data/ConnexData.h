#pragma once
#include <vcruntime.h>
#include <queue>
#include <map>
#include <opencv2/core/mat.hpp>
#include "core/HipeException.h"

namespace filter
{
	
	namespace data
	{
		class DataPortBase
		{
			
		};

		/**
		 * \brief Data port contains the data to garentee the transition with all Ifilter and IModel
		 * \tparam D the Data type to transit between 2 and more connexData
		 */
		template <class D>
		class DataPort : public DataPortBase
		{
		public:
			std::queue<D> data;

			D get()
			{
				return data.front();
			}

			D  pop()
			{
				D res = data.front();
				data.pop();
				return res;
			}

			void push(D dataIn)
			{
				data.push(dataIn);
			}

			inline bool empty() { return data.empty(); }

			inline size_t size() { return data.size(); }
		};

		/**
		 * \brief The enum direction to explain if the input data will modified (INOUT) or read only
		 */
		enum WayData 
		{
			INDATA,
			INOUT
		};

		
		/**
		 * \brief The non template class parent for ConexxData. This class 
		 * is used to contains multiple connector with different type
		 * 
		 */
		class ConnexDataBase
		{
		protected:
			WayData _way;

		public:
			ConnexDataBase(WayData way) : _way(way) {}

			WayData getWay() const
			{
				return _way;
			}

			virtual ConnexDataBase & getCast() 
			{
				return *this;
			}

			inline virtual ConnexDataBase & operator<<(ConnexDataBase &right)
			{
				this->getCast().operator<<(right.getCast());

				return *this;
			}

			virtual DataPortBase &getPort()
			{
				throw HipeException("Cannot down cast to get Port");
			}
		};

		template <class MyClass>
		class CopyObject
		{
		public:
			static MyClass copy(MyClass left)
			{
				MyClass instance = left;
				//TODO For Opencv operator= isn't a copy specialize the copy to avoid the reference
				// and call left.copyTo(instance);

				return instance;
			}


		};


		/**
		 * \brief The static class specialized to restrict the copy to native double array
		 * this is not possible to copy native array without ar
		 * \param left 
		 * \return 
		 */
		template
			double * CopyObject<double *>::copy(double *left);



		/**
		 * \brief The connector is here to connect all data by DataPort. 
		 * \tparam Din the Input data type to accept in the IFiler Object (comming from parents)
		 * \tparam Dout the output data type to push to the next dataPort (going t the childrens)
		 */
		template <class Din, class Dout>
		class ConnexData : public ConnexDataBase
		{
		public:
			
			DataPort<Din> port;

			virtual DataPortBase &getPort()
			{
				return port;
			}

			std::map<ConnexDataBase *, DataPort<Dout> *> portOutput;

			ConnexData() : ConnexDataBase(INDATA)
			{

			}

			ConnexData(WayData way) : ConnexDataBase(way)
			{

			}
			inline bool empty() { return port.empty(); }

			inline size_t size() { return port.size(); }

			Din get()
			{
				return port.get();
			}


			Din pop()
			{
				if (!port.empty())
				{
					Din in = port.pop();
					if (_way == INOUT)
					{
						broacast(static_cast<Dout&>(in));
					}
					return in;
				}

				throw std::runtime_error("No more data to pop");
			}

			void broacast(Dout dataOutput)
			{
				int ref;
				for (auto & childPair : portOutput)
				{
					ConnexDataBase * child = childPair.first;
					/**
					* We need to copy the data to avoid to write on image when other children can use it
					*/
					if (portOutput.size() > 1 && child->getWay() == INOUT)
					{
						portOutput[child]->push(CopyObject<Dout>::copy(dataOutput));

					}
					else
					{
						portOutput[child]->push(dataOutput);
					}
				}
			}

			void push(Dout dataOutput)
			{
				
				if (_way == INOUT)
				{
					throw std::runtime_error("An inout data can't push new data, relation is ONE input to ONE output");
				}

				broacast(dataOutput);

			}

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

			template<class leftIn, class leftOut>
			ConnexData<leftIn, leftOut> & operator<<(ConnexData<leftIn, leftOut> &left)
			{
				this->connectOutput(left);

				return left;
			}

			
			ConnexData<Din, Dout> & operator<<(ConnexDataBase &left)
			{
				portOutput[&left] = static_cast<DataPort<Dout> *>(&(left.getPort()));

				return *this;
			}

			inline virtual ConnexData<Din, Dout> & getCast()
			{
				return *this;
			}



		};


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

			void push(Din dataOutput)
			{

				broacast(dataOutput);

			}

			template<class indata>
			void push(indata matrix)
			{
				Din caps(matrix);
				push(caps);

			}

			//template <>
			void connectInput(ConnexData<Din, Din> &connexIn)
			{
				throw std::runtime_error("Can't add a port there no parent uppper to this object");
			}

			void forwardData()
			{
				if (!port.empty())
				{
					Din in = port.pop();
					if (_way == INOUT)
					{
						broacast(in);
					}
					else
					{
						
					}
				}

				throw std::runtime_error("No more data to pop");
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
				if (!port.empty())
				{
					Dout in = port.pop();
					return in;
				}

				throw std::runtime_error("No more data to pop");
			}

			//template <>
			void ConnectOutput(ConnexData<Dout, Dout> &connexIn)
			{
				throw std::runtime_error("Can't add a port there no children down to this object");
			}

			
		};

	}
}
