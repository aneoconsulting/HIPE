#pragma once
#include <string>
#include "data/IOData.h"
#include "data/DataAccess.h"
#include "data/ConnexData.h"

namespace filter
{
	class	Model
	{
	protected:
		std::string _name;
	
		

	public:
		virtual ~Model() {};

	

		virtual void cleanUp()
		{
			
		}

		virtual void dispose() {}

				
		const std::string & getName() const { return _name; }

		void setName(const std::string& algoName)
		{
			_name = algoName;
		}

	
	};

}
