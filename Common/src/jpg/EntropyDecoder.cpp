#include "jpg/EntropyDecoder.hpp"
#include "BitStream.hpp"
#include "Exception.hpp"

// TMP ////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
// TMP ////////////////////////////////////////////////////

std::vector<int16_t> EntropyDecoder::DecodeBlock(const std::vector<HuffmanCode>& dcTable, const std::vector<HuffmanCode>& acTable)
{
	std::vector<int16_t> block(64); // Initialize all coefficients to zero

	// Decode DC coefficient
	auto dcLength = DecodeValue(dcTable);
std::cout << "Decoded DC Value (length): " << std::dec << dcLength << std::endl;

	int dcValue = Read(dcLength);
	block[0] = dcValue;
std::cout << "Additional bits: " << std::dec << dcValue << std::endl;

	// Decode AC coefficients
	for (int i = 1; i < 64; i++)
	{
		auto acValue = DecodeValue(acTable);
std::cout << "Decoded AC Value: " << std::dec << acValue << std::endl;
		// Valie 0 represents End Of Block (EOB) - all remaining coefficients are zero
		if (acValue == 0)
		{
			break;
		}

		throw RuntimeException("Not implemented");
		// TODO process acValue
		// TODO if 0xF0 -> 16 zeroes
		// TODO else first 4 bits = number of leading zeroes, last 4 bits = length of additional bits
		break;
	}

	return block;
}

uint16_t EntropyDecoder::DecodeValue(const std::vector<HuffmanCode>& huffmanTable)
{
	uint16_t code = 0;

	// loop over code lengths up to maximum
	for (int i = 0; i < huffmanTable[huffmanTable.size() - 1].length; i++)
	{
		uint16_t bit = bitStream.GetNext();
		code = code | bit << i;
//std::cout << "Next bit " << std::dec << (int)bit << std::endl;;
//std::cout << std::dec << "i: " << i << " code " << (int)code << ": ";
//for (int j = 0; j < 16; j++)
//{
//std::cout << ((code & 1 << j) ? "1" : "0");
//if (j == 7) std::cout << " ";
//}
//std::cout << std::endl;

		// Check if code matches any entry in table
		for (const auto& entry : huffmanTable)
		{
			if (entry.length <= i)
			{
				continue;
			}
			else if (entry.length > i + 1)
			{
				break;
			}

if (entry.length != (i + 1))
std::cout << "????????????????????" << std::endl;
			if (entry.length == (i + 1) && entry.code == code)
			{
				return entry.value;
			}
		}
	}

	throw RuntimeException("Failed to decode DC coefficient!");
}

// Read additional bits
// First bit determines the sign
// if 1, number is positive and take as is
// if 0, number is negative and need to be subtracted by (2^value - 1)
int16_t EntropyDecoder::Read(uint16_t length)
{
	int16_t value = 0;
	if (length > 0)
	{
		value = bitStream.GetNext();
		bool isNegative = value == 0; // First bit is a sign
		for (int i = 1; i < length; i++)
		{
			// TODO check if done right!!!
			value = (value << 1) | bitStream.GetNext();
		}

		if (isNegative)
		{
			value = value - ((1 << length) - 1);
		}
	}
	return value;
}