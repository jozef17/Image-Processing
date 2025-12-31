#include "jpg/EntropyDecoder.hpp"
#include "BitStream.hpp"
#include "Exception.hpp"

std::vector<int32_t> EntropyDecoder::DecodeBlock(const std::vector<HuffmanCode>& dcTable, const std::vector<HuffmanCode>& acTable)
{
	std::vector<int32_t> block(64);

	// Decode DC coefficient
	uint8_t dcLength = DecodeValue(dcTable);

	auto dcValue = Read(dcLength);
	block[0] = dcValue;

	// Decode AC coefficients
	for (uint8_t i = 1; i < 64; i++)
	{
		auto acRunLength = DecodeValue(acTable);
		// Valie 0 represents End Of Block (EOB) - all remaining coefficients are zero
		if (acRunLength == 0)
		{
			break;
		}
		else if (acRunLength == 0xF0)
		{
			i += 15; // 16 zeroes (loop increment + 15)
			continue;
		}

		// First 4 bits = number of leading zeroes
		i += (acRunLength >> 4);

		// Last 4 bits = length of additional bits
		auto length = acRunLength & 0x0f;
		auto value = Read(length);
		block[i] = value;
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