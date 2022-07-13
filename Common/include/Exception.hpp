#ifndef EXCEPTION_HPP__
#define EXCEPTION_HPP__

#include <stdexcept>
#include <string>

class Exception : public std::exception
{
public:
	Exception(std::string message) noexcept
		: Exception(message.c_str()) {}

	Exception(char const* const message) noexcept 
		: std::exception(message) {}

};

class RuntimeException : public Exception, public std::runtime_error
{
public:
	RuntimeException(std::string message) noexcept
		: std::runtime_error(message.c_str()), Exception(message) {}

	RuntimeException(char const* const message) noexcept 
		: std::runtime_error(message), Exception(message) {}

};

#endif /* EXCEPTION_HPP__ */