#pragma once

#ifndef HUFFMAN_CODE_HPP__
#define HUFFMAN_CODE_HPP__

#include <cstdint>
#include <vector>

struct HuffmanCode
{
	uint16_t value;
	uint16_t code;
	uint8_t length;

	/// Generates codes for lengths
	static void AsignCodes(std::vector<HuffmanCode>& codes);

	/// Comparison for sorting 
	bool operator<(const HuffmanCode& other) const;

private:
	static std::vector<uint16_t> GetCodes(uint8_t bit, std::vector<uint16_t> oldCodes);
};

using Code = HuffmanCode;

#endif /* HUFFMAN_CODE_HPP__ */