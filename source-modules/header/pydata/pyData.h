//@HIPE_LICENSE@
#pragma once
#include <data/PyContextData.h>
#include <string>

namespace pydata
{
	class pyData
	{
		data::PyContextData _context;

	public:		
		pyData()
		{
			
		}

		pyData(pyData const & right) : _context(right._context)
		{
			
		}

		pyData(data::PyContextData context, std::string jsonArgs = "") : _context(context)
		{
			
		}

		std::string getJsonArgs()
		{

			return "";
		}

	};
}
