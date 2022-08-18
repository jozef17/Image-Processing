#pragma once

#ifndef ADLER_32_HPP__
#define ADLER_32_HPP__

#include <cstdint>
#include <vector>

class Adler32
{
public:
	static uint32_t Decode(const std::vector<uint8_t> &data);

};

#endif /* ADLER_32_HPP__ */