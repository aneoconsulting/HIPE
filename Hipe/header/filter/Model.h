#pragma once
#include <string>
#include "data/IOData.h"
#include "data/DataAccess.h"

namespace filter
{
	class	Model
	{
	protected:
		std::string _name;
		data::IOData _data;
		DataAccess _protect;

	public:
		virtual ~Model() {};

		DataAccess get_protect() const
		{
			return _protect;
		}

		void set_protect(const DataAccess data_access)
		{
			_protect = data_access;
		}

		void setInputData(const data::IOData & inputData)
		{
			if (_data.empty())
			{
				_data = inputData;
			} 
			else
			{
				_data << inputData;
			}
			
		}
		
		std::string getName() const
		{
			return _name;
		}

		void setName(const std::string& algoName)
		{
			_name = algoName;
		}

	
	};

}
