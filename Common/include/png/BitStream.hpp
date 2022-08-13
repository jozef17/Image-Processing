#pragma once

#ifndef BIT_STREAM_HPP__
#define BIT_STREAM_HPP__

#include <vector>
#include <memory>
#include <cstdint>

class BitStream
{
public:

	/// <summary>
	/// Returns next bit in stream
	/// </summary>
	bool GetNext();

	/// <summary>
	/// Returns current byte and moves cursor at the 
	/// beggining of next byte
	/// </summary>
	uint8_t GetCurrentByte();

	bool& operator>>(bool& b);

	uint8_t& operator>>(uint8_t& b);

	/// <summary>
	/// Skips next <bitsToSkip> bits
	/// </summary>
	void Skip(uint32_t bitsToSkip);

	/// <summary>
	/// Appends data to the end of stream
	/// </summary>
	/// <param name="data">Data to be appended</param>
	/// <param name="size">Length of the data in bytes</param>
	void Append(std::unique_ptr<uint8_t[]> data, uint32_t length);

private:
	uint32_t bytePosition  = 0;
	uint8_t  bitPosition   = 0;
	uint8_t  arrayPosition = 0;

	std::vector<std::unique_ptr<uint8_t[]>> data;
	std::vector<uint32_t> lengths;
};

#endif /* BIT_STREAM_HPP__ */