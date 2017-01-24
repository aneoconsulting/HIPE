#pragma once

#include <exception>
#include <string>

class HipeException : public std::exception {

		std::string message;

	public:
		HipeException(const char * message);

		HipeException(std::string message);

	public:
		const char* what() const noexcept override;

};
