#pragma once

#ifndef UTILS_HPP__
#define UTILS_HPP__

#include <string>

class Utils
{
public:
	Utils() = delete;

	static bool EndsWith(const std::string& str, const std::string& ending) noexcept;

};

#endif /* UTILS_HPP__ */