#include "HuffmanCode.hpp"

#include <algorithm>

void HuffmanCode::AsignCodes(std::vector<HuffmanCode>& codes)
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
		for (auto& code : codes)
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

std::vector<uint16_t> HuffmanCode::GetCodes(uint8_t bit, std::vector<uint16_t> oldCodes)
{
	std::vector<uint16_t> ret;
	for (auto val : oldCodes)
	{
		ret.push_back(val);
		ret.push_back(val | 1 << bit);
	}
	return ret;
}

bool HuffmanCode::operator<(const HuffmanCode& other) const
{
	if (this->length == other.length)
	{
		return this->value < other.value;
	}
	return this->length < other.length;
}
