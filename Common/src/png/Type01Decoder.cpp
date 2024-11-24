#include "png/Type01Decoder.hpp"
#include "png/Adler32.hpp"
#include "BitStream.hpp"
#include "Exception.hpp"

#ifdef ENABLE_LOGS
#include <iostream>
#include <iomanip>
#define LOG(msg) std::cout << msg
#else
#define LOG(msg)
#endif

Type01Decoder::Type01Decoder(BitStream& bitstream) : HuffmanDecoder(bitstream)
{}

uint16_t Type01Decoder::GetLLCode()
{
	// Read first 7 bits
	auto bit6 = this->bitstream.GetNext();
	auto bit5 = this->bitstream.GetNext();
	auto bit4 = this->bitstream.GetNext();
	auto bit3 = this->bitstream.GetNext();
	auto bit2 = this->bitstream.GetNext();
	auto bit1 = this->bitstream.GetNext();
	auto bit0 = this->bitstream.GetNext();

	uint16_t code = (int)bit0 | ((int)bit1 << 1) | ((int)bit2 << 2) | ((int)bit3 << 3) | ((int)bit4 << 4) | ((int)bit5 << 5) | ((int)bit6 << 6);
	if ((code <= 23)) // 7bit codes: 256 - 279 
	{
		code += 256;
		LOG("[Type01Decoder::GetLLCode] llcode: " << std::setw(3) << std::setfill('0') << std::dec << (int)code << " " 
			<< bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl);
		return code;
	}

	// Get 8th bit
	auto bit7 = this->bitstream.GetNext();
	code = (code << 1) | ((int)bit7);
	if ((code >= 48) && (code <= 191)) // 8bit codes: 0 - 143
	{
		code -= 48;
		LOG("[Type01Decoder::GetLLCode] llcode: " << std::setw(3) << std::setfill('0') << std::dec << (int)code << " "
			<< bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << bit7 << std::endl);
		return code;
	}
	else if ((code >= 192) && (code <= 199)) // 8bit codes: 280 - 287
	{
		code -= 88;
		LOG("[Type01Decoder::GetLLCode] llcode: " << std::setw(3) << std::setfill('0') << std::dec << (int)code << " "
			<< bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << bit7 << std::endl);
		return code;
	}

	// 9bit 144 - 255
	auto bit8 = this->bitstream.GetNext();
	code = (code << 1) | ((int)bit8);
	if (code < 400)
	{
		throw RuntimeException("Invalid literal/length code " + std::to_string(code));
	}
	code -= 256;
	LOG("[Type01Decoder::GetLLCode] llcode: " << std::setw(3) << std::setfill('0') << std::dec << (int)code << " "
		<< bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << bit7 << bit8 << std::endl);
	return code;
}

uint16_t Type01Decoder::GetDistanceCode()
{
	// Distance code has 5 bits
	auto distBit4 = this->bitstream.GetNext();
	auto distBit3 = this->bitstream.GetNext();
	auto distBit2 = this->bitstream.GetNext();
	auto distBit1 = this->bitstream.GetNext();
	auto distBit0 = this->bitstream.GetNext();

	int distCode = (int)distBit0 | ((int)distBit1 << 1) | ((int)distBit2 << 2) | ((int)distBit3 << 3) | ((int)distBit4 << 4);
	LOG("[Type01Decoder::GetDistanceCode] distCode: " << std::setw(3) << std::setfill('0') << std::dec << (int)distCode 
		<< " " << distBit4 << distBit3 << distBit2 << distBit1 << distBit0 << std::endl);


	// Invalid distance code check
	if ((distCode == 30) || (distCode == 31))
	{
		throw RuntimeException("Invalid distance code " + std::to_string(distCode));
	}
	return static_cast<uint8_t>(distCode);
}
