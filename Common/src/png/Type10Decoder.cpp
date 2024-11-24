#include "png/Type10Decoder.hpp"
#include "png/Adler32.hpp"
#include "Exception.hpp"
#include "BitStream.hpp"

#include <algorithm>

#ifdef ENABLE_LOGS
#include <iostream>
#endif

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

#ifdef ENABLE_LOGS
	std::cout << "[Code::AsignCodes]" << std::endl;
	for (auto a : codes)
	{
		std::cout << "                   " << std::dec << (int)a.value << " " << std::dec << (int)a.length << "  ";
		for (int bit = 0; bit < a.length; bit++)
		{
			bool b = (a.code & 1 << bit) >> bit;
			std::cout << std::dec << (int)b;
		}
		std::cout << std::endl;
	}
#endif
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

Type10Decoder::Type10Decoder(BitStream& bitstream) : HuffmanDecoder(bitstream)
{
	uint8_t hlit  = static_cast<uint8_t>(Get(5));
	uint8_t hdist = static_cast<uint8_t>(Get(5));
	uint8_t hclen = static_cast<uint8_t>(Get(4));
#ifdef ENABLE_LOGS
	std::cout << "[Type10Decoder::Type10Decoder] - HLIT : " << std::dec << (int)hlit << std::endl;
	std::cout << "                               - HDIST: " << std::dec << (int)hdist << std::endl;
	std::cout << "                               - HCLEN: " << std::dec << (int)hclen << std::endl;
#endif

	// Get alphabets
	// Read and process code lengths
	auto codeLengths = GetCodeLengths(hclen);

	// Decode literal length alphabet
	this->llAphabet = GetAlphabet(hlit + 257, codeLengths);
#ifdef ENABLE_LOGS
	std::cout << "[Type10Decoder::Type10Decoder] Literal/Length codes: " << std::endl;
	for (auto& code : this->llAphabet)
	{	
		std::cout << "                               - " << (int)code.value << " (" << (int)code.length << ") ";
		for (int i = 0; i < code.length; i++)
		{
			std::cout << (int)((code.code & 1 << i) >> i);
		}
		std::cout << std::endl;
	}
#endif

	// Decode distance alphabet
	this->distanceAlphabet = GetAlphabet(hdist + 1, codeLengths);
#ifdef ENABLE_LOGS
	std::cout << "[Type10Decoder::Type10Decoder] Distance codes: " << std::endl;
	for (auto& code : this->distanceAlphabet)
	{
		std::cout << "                               - " << (int)code.value << " (" << (int)code.length << ") ";
		for (int i = 0; i < code.length; i++)
		{
			std::cout << (int)((code.code & 1 << i) >> i);
		}
		std::cout << std::endl;
	}
#endif
}

Type10Decoder::~Type10Decoder()
{}

uint16_t Type10Decoder::GetLLCode()
{
	return Get(this->llAphabet);
}

uint16_t Type10Decoder::GetDistanceCode()
{
	return Get(this->distanceAlphabet);
}

uint16_t Type10Decoder::Get(uint8_t numOfBits)
{
	uint16_t value = 0;
	for (uint8_t i = 0; i < numOfBits; i++)
	{
		auto bit = this->bitstream.GetNext();
		value = value | bit << i;
	}
	return value;
}

uint16_t Type10Decoder::Get(const std::vector<Code>& codes)
{
	auto length = codes.at(0).length;
	auto value = Get(length);
	for (const auto& code : codes)
	{
		if (code.length > length)
		{
			for (length; length < code.length; length++)
			{
				auto bit = this->bitstream.GetNext();
				value = value | bit << length;
			}
		}

		if (code.code == value)
		{
			return code.value;
		}
	}

	throw RuntimeException("Decoding Failed");
}

std::vector<Code> Type10Decoder::GetCodeLengths(uint8_t hclen)
{
	uint8_t indexes[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

	// Read lengths
	std::vector<Code> codeLengths;
	for (uint8_t i = 0; i < hclen + 4; i++)
	{
		auto length = Get(3);
		if (length > 0)
		{
			Code code = { 0 };
			code.value = indexes[i];
			code.length = static_cast<uint8_t>(length);
			codeLengths.push_back(code);
		}
	}
	std::sort(codeLengths.begin(), codeLengths.end());

	// Asign codes
	Code::AsignCodes(codeLengths);

#ifdef ENABLE_LOGS
	std::cout << "[Type10Decoder::GetCodeLengths] Code Lengths: ";
	for (uint8_t i = 0; i < codeLengths.size(); i++)
	{
		std::cout << std::dec << "(" << (int)codeLengths[i].value << ")" << (int)codeLengths[i].length << " ";
	}
	std::cout << std::endl;
#endif
	return codeLengths;
}

std::vector<Code> Type10Decoder::GetAlphabet(uint16_t numElements, const std::vector<Code>& distCodes)
{
	std::vector<Code> codes;

	while (codes.size() < numElements )
	{
		auto val = Get(distCodes);

		if (val < 16)
		{
			Code code = { 0 };
			code.length = static_cast<uint8_t>(val);
			code.value = static_cast<uint16_t>(codes.size());
			codes.push_back(code);
#ifdef ENABLE_LOGS
			std::cout << "[Type10Decoder::GetAlphabet] Adding length: " << (int)val << std::endl;
#endif
		}
		if (val == 16)
		{
			auto repeatLength = Get(2) + 3;
			auto code = codes.at(codes.size() - 1);
#ifdef ENABLE_LOGS
			std::cout << "[Type10Decoder::GetAlphabet] (16) Adding " 
				<< std::dec << (int)(code.length) << " " 
				<< (int)repeatLength << " times" << std::endl;
#endif
			for (uint16_t i = 0; i < repeatLength; i++)
			{
				code.value = static_cast<uint16_t>(codes.size());
				codes.push_back(code);
			}
		}
		else if (val == 17)
		{

			auto repeatLength = Get(3) + 3;
#ifdef ENABLE_LOGS
			std::cout << "[Type10Decoder::GetAlphabet] (17) Adding "
				<< std::dec << (int)repeatLength << " zeroes" << std::endl;
#endif
			Code code = { 0 };
			for (uint16_t i = 0; i < repeatLength; i++)
			{
				code.value = static_cast<uint8_t>(codes.size());
				codes.push_back(code);
			}
		}
		else if (val == 18)
		{
			auto repeatLength = Get(7) + 11;
#ifdef ENABLE_LOGS
			std::cout << "[Type10Decoder::GetAlphabet] (18) Adding "
				<< std::dec << (int)repeatLength << " zeroes" << std::endl;
#endif
			Code code = { 0 };
			for (uint16_t i = 0; i < repeatLength; i++)
			{
				code.value = static_cast<uint16_t>(codes.size());
				codes.push_back(code);
			}
		}
	}

	// Remove 0 lengths
	codes.erase(
		std::remove_if( 
			codes.begin(), 
			codes.end(), 
			[](const Code& code){return code.length == 0;}
		),
		codes.end()
	);

	std::sort(codes.begin(), codes.end());
	Code::AsignCodes(codes);

	return codes;
}
