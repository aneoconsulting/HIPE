#pragma once

#include <exception>
#include <string>
#include <core/osdependant.h>

class HipeException : public std::exception {

		std::string message;

	public:
		HipeException(const char * message);

		HipeException(std::string message);

	public:
		const char* what() const NO_EXCEPT override;

};
