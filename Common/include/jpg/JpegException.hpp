#pragma once

#include "Exception.hpp"

#ifndef JPEG_EXCEPTION_HPP__
#define JPEG_EXCEPTION_HPP__

class JpegException : public RuntimeException
{
public:
	JpegException(const std::string& message)
		: RuntimeException(message) {}
};

#endif // JPEG_EXCEPTION_HPP__