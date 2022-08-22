#include "png/Type10Decoder.hpp"
#include "png/BitStream.hpp"
#include "png/Adler32.hpp"
#include "Exception.hpp"

#include <algorithm>

#include <iostream>

struct Code
{
	uint16_t value;
	uint16_t code;
	uint8_t length;

	/// Generates codes for lengths
	static void AsignCodes(std::vector<Code>& codes);

	/// Comparison for sorting 
	bool operator<(const Code& other) const;

private:
	static std::vector<uint16_t> GetCodes(uint8_t bit, std::vector<uint16_t> oldCodes);
};

void Code::AsignCodes(std::vector<Code>& codes)
{
	uint8_t minLength = std::min_element(codes.begin(), codes.end())->length;
	uint8_t maxLength = std::max_element(codes.begin(), codes.end())->length;

	// TODO check if uint16_t is enough 
	std::vector<uint16_t> values = { 0, 1 };
	for (uint8_t i = 1; i < minLength; i++)
	{
		values = GetCodes(i, values);
	}

	for (uint8_t i = minLength; i <= maxLength; i++)
	{
		for(auto &code : codes)
		{
			if (code.length != i)
			{
				continue;
			}

			code.code = values[0];
			values.erase(values.begin());
		}
		values = GetCodes(i, values);
	}

	for (auto a : codes)//////////////////////////////////////////////////////////
	{/////////////////////////////////////////////////////////////////////////////
		std::cout << std::dec << (int)a.value << " " << std::dec << (int)a.length << "  ";
		for (int bit = 0; bit < a.length; bit++)/////////////////////////////////////////
		{/////////////////////////////////////////////////////////////////////////
			bool b = (a.code & 1 << bit) >> bit;//////////////////////////////////
			std::cout << std::dec << (int)b;//////////////////////////////////////
		}/////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;///////////////////////////////////////////////////
	}/////////////////////////////////////////////////////////////////////////////

}

std::vector<uint16_t> Code::GetCodes(uint8_t bit, std::vector<uint16_t> oldCodes)
{
	std::vector<uint16_t> ret;
	for (auto val : oldCodes)
	{
		ret.push_back(val);
		ret.push_back(val | 1 << bit);
	}
	return ret;
}

bool Code::operator<(const Code& other) const
{
	if (this->length == other.length)
	{
		return this->value < other.value;
	}
	return this->length < other.length;
}

Type10Decoder::Type10Decoder(BitStream& bitstream) : bitstream(bitstream)
{}

std::vector<uint8_t> Type10Decoder::Decode()
{
	std::vector<uint8_t> data;
	std::cout << "compressed with dynamic Huffman codes" << std::endl;

	// 5 Bits: HLIT, # of Literal / Length codes - 257 (257 - 286)
	// 5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
	// 4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)
	/////////////////////////////////////////////////////////////////////////////
	std::cout << std::dec << "HLIT : ";//////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	uint8_t hlit = Get(5);
	/////////////////////////////////////////////////////////////////////////////
	std::cout << " " << std::dec << (int)hlit << std::endl;////////////////////////////////////
	std::cout << std::dec << "HDIST: ";//////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	uint8_t hdist = Get(5);
	/////////////////////////////////////////////////////////////////////////////
	std::cout << " " << std::dec << (int)hdist << std::endl;////////////////////////////////////
	std::cout << std::dec << "HCLEN: ";//////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	uint8_t hclen = Get(4);
	/////////////////////////////////////////////////////////////////////////////
	std::cout << " " << std::dec << (int)hclen << std::endl;////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	// Get alphabets
	// Read and process code lengths
	auto codeLengths = GetCodeLength(hclen);

	// Decode literal length alphabet
	auto llCodes = GetLiteralLengthCodes(hlit, codeLengths);

	// Decode distance alphabet
	auto distCodes = GetDistanceCodes(hdist, codeLengths);

	while(true)
	{
		// The actual compressed data of the block, encoded using the 
		// literal / length and distance Huffman codes
		// TODO

		// The literal / length symbol 256 (end of data), encoded 
		// using the literal / length Huffman code
		// TODO

		// ?????????????????????????????????????????????????????????????????
		// The code length repeat codes can cross from HLIT + 257 to the
		// HDIST + 1 code lengths.In other words, all code lengths form
		// a single sequence of HLIT + HDIST + 258 values.
		break;
	}

	return data;
}

uint16_t Type10Decoder::Get(uint8_t numOfBits)
{
	uint16_t value = 0;
	for (uint8_t i = 0; i < numOfBits; i++)
	{
		auto bit = this->bitstream.GetNext();
		//		value = value << 1 | bit;  // ?????
		value = value | bit << i;  // ?????
		////////////////////////////////////////////////////////////////////////////
		std::cout << (int)bit;//////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
	}
	return value;
}

uint16_t Type10Decoder::Get(const std::vector<Code>& codes)
{
	uint8_t length = codes.at(0).length;
	auto value = Get(length);
	for (const auto& code : codes)
	{
		if (code.length > length)
		{
			for (length; length < code.length; length++)
			{
				auto bit = this->bitstream.GetNext();
//				value = value << 1 | bit;  // ?????
				value = value | bit << length;  // ?????
				////////////////////////////////////////////////////////////////////////////
				std::cout << (int)bit;//////////////////////////////////////////////////////
				////////////////////////////////////////////////////////////////////////////

			}
		}

		if (code.code == value)
		{
			return code.value;
		}
	}

	throw RuntimeException("Decoding Failed");
}

std::vector<Code> Type10Decoder::GetCodeLength(uint8_t hclen)
{
	// (HCLEN + 4) x 3 bits: code lengths for the code length alphabet given just
	// above, in the order : 
	//           16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
	// These code lengths are interpreted as 3 - bit integers (0 - 7); as above, 
	// a code length of 0 means the corresponding symbol(literal / length or 
	// distance code length) is not used.
	uint8_t indexes[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

	// Read lengths
	std::vector<Code> codeLengths;
	for (uint8_t i = 0; i < hclen + 4; i++)
	{
		auto length = Get(3);
		////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << " ";///////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////
		if (length > 0)
		{
			Code code = { 0 };
			code.value = indexes[i];
			code.length = length;
			codeLengths.push_back(code);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::endl;///////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::sort(codeLengths.begin(), codeLengths.end());

	// Asign codes
	Code::AsignCodes(codeLengths);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "Code Lengths: ";/////////////////////////////////////////////////////////////////////////
	for (uint8_t i = 0; i < codeLengths.size(); i++)///////////////////////////////////////////////////////////////////////
	{//////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::dec << "(" << (int)codeLengths[i].value << ")" << (int)codeLengths[i].length << " ";
	}//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::endl;////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	return codeLengths;
}

// decodes literal length codes
std::vector<Code> Type10Decoder::GetLiteralLengthCodes(uint8_t hlit, const std::vector<Code> &distCodes)
{
	std::vector<Code> codes;

	// HLIT + 257 code lengths for the literal / length alphabet,
	//	encoded using the code length Huffman code

	while (codes.size() < hlit + 257)
	{
		auto val = Get(distCodes);
		/////////////////////////////////////////////////////////////////////////////
		std::cout << " " << std::dec << (int)val << std::endl;////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////

		if (val < 16)
		{
			Code code = { 0 };
			code.length = val;
			code.code = codes.size() - 1;
			codes.push_back(code);

			continue;
		}
		if (val == 16)
		{
			// 16: Copy the previous code length 3 - 6 times.
			// 	The next 2 bits indicate repeat length (0 = 3, ..., 3 = 6)
			// 	Example:  Codes 8, 16 (+2 bits 11),
			// 	16 (+2 bits 10) will expand to
			// 	12 code lengths of 8 (1 + 6 + 5)
			
			// TODO
		}
		else if (val == 17)
		{
			// 17: Repeat a code length of 0 for 3 - 10 times. (3 bits of length)
			// TODO
		}
		else if (val == 18)
		{
			// 18: Repeat a code length of 0 for 11 - 138 times (7 bits of length)
			// TODO
		}
		break;
	}

	return codes;
}

// decodes distance codes
std::vector<Code> Type10Decoder::GetDistanceCodes(uint8_t hdist, const std::vector<Code> &distCodes)
{
	std::vector<Code> codes;
	// HDIST + 1 code lengths for the distance alphabet, encoded using the code length Huffman code

	// TODO

	return codes;
}
