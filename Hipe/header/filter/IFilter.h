#pragma once

#include <string>

namespace filter {
	class IFilter
	{
	public:
		IFilter() {}

	public:
		virtual void getNextFilter();

		virtual void getPreviousFilter();

		virtual void getRootFilter();

		virtual void getParent();

		virtual void getNextChildren();

		virtual std::string resultAsString() = 0;

	};
}
