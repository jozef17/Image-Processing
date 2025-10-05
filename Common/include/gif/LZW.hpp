#pragma once

#ifndef LZW_HPP__
#define LZW_HPP__

#include <vector>
#include <cstdint>

class BitStream;

class LZW 
{
public:
	LZW(BitStream& stream, uint8_t lzwMinCodeSize, uint16_t tableSize)
		: stream(stream), lzwMinCodeSize(lzwMinCodeSize), tableSize(tableSize){};

	std::vector<uint8_t> Decode();

private:
	BitStream& stream;
	uint8_t lzwMinCodeSize;
	uint16_t tableSize;
};

#endif /* LZW_HPP__ */