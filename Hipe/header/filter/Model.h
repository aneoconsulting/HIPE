#pragma once
#include <string>

namespace filter
{
	class Model
	{
	protected:
		std::string _name;

	public:
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