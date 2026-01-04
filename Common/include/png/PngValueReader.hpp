#pragma once

#ifndef PNG_VALUE_READER__
#define PNG_VALUE_READER__

#include <cstdint>
#include <vector>

class PngValueReader final
{
public:
	/// <summary>
	/// Initializes reader with decoded DEFLATE values and the channel / value size
	/// </summary>
	/// <param name="decodedValues">DEFLATE decoded values</param>
	/// <param name="channelSize">channel / value size</param>
	PngValueReader(std::vector<uint8_t> &&ecodedValues, uint8_t channelSize);

	/// <summary>
	/// Reads next value based on channel size
	/// </summary>
	/// <returns>next value based on channel size</returns>
	uint16_t GetNextValue();
	
	/// <summary>
	/// Moves to next whole byte and returns its vallue
	/// </summary>
	/// <returns>next whole byte</returns>
	uint8_t GetFilterMethod();

private:
	/// <summary>
	/// Decoded deflate stream
	/// </summary>
	std::vector<uint8_t> decodedValues;

	uint32_t nextByte = 0;
	uint8_t  nextBit = 0;

	/// <summary>
	/// Channel / value size
	/// </summary>
	uint8_t channelSize;
};

#endif // VALUE_READER__
