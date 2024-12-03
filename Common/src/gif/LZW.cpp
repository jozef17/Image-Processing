#include "gif/LZW.hpp"
#include "BitStream.hpp"

#include <map>
#include <string>

std::vector<uint8_t> LZW::Decode()
{
	std::map<uint32_t, std::vector<uint32_t>> decodeTable;

	uint32_t prewCode = (uint32_t)-1;
	uint32_t nextCodeIndex = 0;
	uint32_t codeSize = lzwMinCodeSize;
	uint32_t maxCode = (1 << (codeSize + 1));
	std::vector<uint8_t> imageColorIndexes;

	// Decode
	while (true)
	{
		uint32_t code = 0;
		for (uint32_t i = 0; i <= codeSize; i++)
		{
			uint32_t bit = this->stream.GetNext() ? 1 : 0;
			code = code | (bit << i);
		}

		// End of data!
		if (code == this->tableSize + 1)
		{
			break;
		}
		// Clear / reset
		else if (code == this->tableSize)
		{
			decodeTable.clear();

			// Re-generate Decode table
			for (uint32_t i = 0; i < this->tableSize; i++)
			{
				decodeTable[i] = { i }; // Clear / Reset code
			}
			nextCodeIndex = (uint32_t)this->tableSize + 2;
			prewCode = (uint32_t)-1;
			codeSize = this->lzwMinCodeSize;
			maxCode = (1 << (codeSize + 1));
			continue;
		}

		// Output first color after clear
		if (prewCode == (uint32_t)-1)
		{
			imageColorIndexes.push_back(code);
			prewCode = code;
			continue;
		}

		if (decodeTable.find(code) != decodeTable.end())
		{
			// Code in table
			std::vector<uint32_t> pattern = decodeTable[code];
			imageColorIndexes.insert(imageColorIndexes.end(), pattern.begin(), pattern.end());

			// Add code to table
			auto first = pattern.at(0); // K
			std::vector<uint32_t> newPattern = decodeTable[prewCode];
			newPattern.push_back(first);
			decodeTable[nextCodeIndex++] = newPattern;
		}
		else if (code != nextCodeIndex)
		{
			throw("Invaid code " + std::to_string(code) + " (" + std::to_string(nextCodeIndex) + ")!!!");
		}
		else
		{
			// Code not in table;
			auto firstPrew = decodeTable[prewCode].at(0); // K
			std::vector<uint32_t> newPattern = decodeTable[prewCode];
			newPattern.push_back(firstPrew);
			decodeTable[nextCodeIndex++] = newPattern;
			imageColorIndexes.insert(imageColorIndexes.end(), newPattern.begin(), newPattern.end());
		} // new / old code
		prewCode = code;

		if (nextCodeIndex == maxCode)
		{
			codeSize++;
			maxCode = (1 << (codeSize + 1));
		}
	} // loop

	return imageColorIndexes;
}