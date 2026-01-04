#pragma once

#ifndef ENTROPY_DECODER_HPP__
#define ENTROPY_DECODER_HPP__

#include <vector>
#include "HuffmanCode.hpp"

class BitStream;

/// <summary>
/// Decoder for JPEG entropy encoded data
/// </summary>
class EntropyDecoder final
{
public:
	explicit EntropyDecoder(BitStream& bitStream) : bitStream(bitStream) {};

	/// <summary>
	/// Decode single 8x8 block from bit stream for provided component
	/// </summary>
	/// <param name="component">Color component to be decoded</param>
	/// <returns>Decoded 8x8 block of data. Note: First Element contains decoded DC to be updated according needs</returns>
	std::vector<int32_t> DecodeBlock(const std::vector<HuffmanCode>& dcTable, const std::vector<HuffmanCode>& acTable);

private:
	/// <summary>
	/// Decode single value from bit stream using provided huffman table
	/// </summary>
	/// <param name="huffmanTable">Huffman Table to be used for decoding</param>
	/// <returns>Decoded value, an exception is thrown when decoding fails</returns>
	uint8_t DecodeValue(const std::vector<HuffmanCode>& huffmanTable);

	/// <summary>
	/// Reads given number of DC bits from the stream and converts them to signed integer
	/// </summary>
	/// <param name="size">Number of bits to read</param>
	/// <returns></returns>
	int32_t Read(uint16_t size);

	BitStream& bitStream;
};


#endif /* ENTROPY_DECODER_HPP__ */