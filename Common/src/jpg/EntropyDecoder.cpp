#include "jpg/EntropyDecoder.hpp"
#include "BitStream.hpp"
#include "Exception.hpp"

// TMP ////////////////////////////////////////////////////
#include <iostream>
#include <iomanip>
// TMP ////////////////////////////////////////////////////

std::vector<int32_t> EntropyDecoder::DecodeBlock(const std::vector<HuffmanCode>& dcTable, const std::vector<HuffmanCode>& acTable)
{
	std::vector<int32_t> block(64); // Initialize all coefficients to zero

	// Decode DC coefficient
	uint8_t dcLength = DecodeValue(dcTable);
std::cout << "Decoded DC Value (length): " << std::dec << (int)dcLength << std::endl;

	auto dcValue = Read(dcLength);
	block[0] = dcValue;
std::cout << "Additional bits: " << std::dec << (int)dcValue << std::endl;

	// Decode AC coefficients
	for (uint8_t i = 1; i < 64; i++)
	{
		auto acRunLength = DecodeValue(acTable);
std::cout << "Decoded AC Value: " << std::dec << (int)acRunLength << std::endl;
		// Valie 0 represents End Of Block (EOB) - all remaining coefficients are zero
		if (acRunLength == 0)
		{
			break;
		}
		else if (acRunLength == 0xF0)
		{
			throw RuntimeException("AC coefficient: 0xF0 encountered!");
			// 16 zeroes
			i += 15; // TODO 15 or 16?
			continue;
		}

		// First 4 bits = number of leading zeroes
		uint8_t zeroesRun = acRunLength >> 4;
		i += zeroesRun;

		// Last 4 bits = length of additional bits
		auto length = acRunLength & 0x0f;
		auto value = Read(length);
		block[i] = value;
std::cout << "  - adding #zeroes " << std::dec << (int)zeroesRun << std::endl;
std::cout << "  - ac run         " << std::dec << (int)length << std::endl;
std::cout << "  - value          " << std::dec << (int)value << std::endl;
	}

	return block;
}

uint8_t EntropyDecoder::DecodeValue(const std::vector<HuffmanCode>& huffmanTable)
{
	uint16_t code = 0;

	// loop over code lengths up to maximum
	for (uint8_t i = 0; i < huffmanTable[huffmanTable.size() - 1].length; i++)
	{
		uint32_t bit = bitStream.GetNext();
		code = code | (bit << i);
/*std::cout << "Next bit " << std::dec << (int)bit << std::endl;;
std::cout << std::dec << "i: " << (int)i << " code " << (int)code << ": ";
for (int j = 0; j < 16; j++)
{
std::cout << ((code & 1 << j) ? "1" : "0");
if (j == 7) std::cout << " ";
}
std::cout << std::endl;//*/

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
				return static_cast<uint8_t>(entry.value);
			}
		}
	}

	throw RuntimeException("Failed to decode coefficient!");
}

// Read additional bits
// First bit determines the sign
// if 1, number is positive and take as is
// if 0, number is negative and need to be subtracted by (2^value - 1)
int32_t EntropyDecoder::Read(uint16_t length)
{
	int32_t value = 0;
	if (length > 0)
	{
		value = bitStream.GetNext();
		bool isNegative = value == 0; // First bit is a sign
		for (uint16_t i = 1; i < length; i++)
		{
			value = (value << 1) | bitStream.GetNext();
		}

		if (isNegative)
		{
			value = value - (1 << length) + 1;
		}
	}
	return value;
}