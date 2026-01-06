#ifndef EXCEPTION_HPP__
#define EXCEPTION_HPP__

#include <stdexcept>
#include <string>

class Exception : public std::runtime_error
{
public:
	Exception(const std::string &message) noexcept
		: std::runtime_error(message.c_str()) {}

};

class RuntimeException : public Exception
{
public:
	RuntimeException(const std::string &message) noexcept
		: Exception(message) {}

};

#endif /* EXCEPTION_HPP__ */