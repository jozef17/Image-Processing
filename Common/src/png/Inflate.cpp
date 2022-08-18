#include "png/Inflate.hpp"
#include "png/BitStream.hpp"
//#include "png/Type00Decoder.hpp"
#include "png/Type01Decoder.hpp"
#include "png/Type10Decoder.hpp"
#include "Exception.hpp"

#include <iostream>
#include <iomanip>
#include <bitset>

Inflate::Inflate(BitStream& bitstream) noexcept : bitstream(bitstream)
{}

std::vector<uint8_t> Inflate::Decode()
{
	std::vector<uint8_t> data;

	auto idatHdr1 = bitstream.GetCurrentByte();
	auto idatHdr2 = bitstream.GetCurrentByte();

	// deflate / inflate supports ony compression method - 0x08 "deflate"
	auto compressionMethod = idatHdr1 & 0x0F;
	if (compressionMethod != 0x08)
	{
		throw RuntimeException("Unsupported compression method: \"" + std::to_string(compressionMethod) + "\"");
	}

	/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "- data 0    " << std::setw(2) << std::setfill('0') << std::hex << (int)idatHdr1 << " (" << std::bitset<8>(idatHdr1) << ")" << std::endl;////
	std::cout << "- data 1    " << std::setw(2) << std::setfill('0') << std::hex << (int)idatHdr2 << " (" << std::bitset<8>(idatHdr2) << ")" << std::endl;////
	std::cout << "   - Compression method " << std::setw(2) << std::setfill('0') << std::dec << (int)compressionMethod << std::endl;//////////////////////////
	std::cout << "   - Compression info   " << std::setw(2) << std::setfill('0') << std::dec << (int)(idatHdr1 >> 4) << std::endl;////////////////////////////
	std::cout << "   - F Check            " << std::setw(2) << std::setfill('0') << std::dec << (int)(idatHdr2 & 0x1f) << std::endl;//////////////////////////
	std::cout << "   - F DICT             " << std::setw(2) << std::setfill('0') << std::dec << (int)((idatHdr2 >> 5) & 0x1) << std::endl;////////////////////
	std::cout << "   - F Level            " << std::setw(2) << std::setfill('0') << std::dec << (int)(idatHdr2 >> 6) << std::endl;////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

	bool isLast = false;
	do
	{
		// Handle block header
		isLast = bitstream.GetNext();
		auto decoder = GetDecoder();
		// Decode block
		auto newData = decoder->Decode();
		data.insert(data.end(), newData.begin(), newData.end());
	} while (!isLast);

	return data;
}

std::unique_ptr<BlockDecoder> Inflate::GetDecoder()
{
	auto typeBit1 = bitstream.GetNext();
	auto typeBit2 = bitstream.GetNext();

	if (!typeBit2 && !typeBit1) // 00
	{
		std::cout << "No Compression" << std::endl;
		throw RuntimeException("Not Implemented"); // TODO
	}
	else if (!typeBit2 && typeBit1) // 01
	{
		return std::unique_ptr<BlockDecoder>(new Type01Decoder(this->bitstream));
	}
	else if (typeBit2 && !typeBit1) // 10
	{
		return std::unique_ptr<BlockDecoder>(new Type10Decoder(this->bitstream));
	}
	else if (!typeBit2 && typeBit1) // 11
	{
		throw RuntimeException("Invalid DEFLATE type specifier");
	}
}
